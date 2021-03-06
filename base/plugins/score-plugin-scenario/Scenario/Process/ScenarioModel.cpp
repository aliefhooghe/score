// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ScenarioModel.hpp"

#include "Algorithms/StandardCreationPolicy.hpp"

#include <Process/Process.hpp>
#include <Process/TimeValue.hpp>
#include <Scenario/Commands/Scenario/Displacement/MoveEventMeta.hpp>
#include <Scenario/Document/CommentBlock/CommentBlockModel.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/Interval/IntervalDurations.hpp>
#include <Scenario/Document/Interval/IntervalModel.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/TimeSync/TimeSyncModel.hpp>
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <Scenario/Process/Algorithms/ProcessPolicy.hpp>
#include <Scenario/Process/ScenarioProcessMetadata.hpp>

#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <score/document/DocumentContext.hpp>
#include <score/document/DocumentInterface.hpp>
#include <score/model/EntityMap.hpp>
#include <score/model/ModelMetadata.hpp>
#include <score/selection/Selectable.hpp>
#include <score/selection/SelectionDispatcher.hpp>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/tools/IdentifierGeneration.hpp>
#include <score/tools/Todo.hpp>

#include <core/document/Document.hpp>

#include <QDataStream>
#include <QDebug>
#include <QIODevice>
#include <QMap>
#include <QtGlobal>

#include <wobjectimpl.h>

#include <vector>
W_OBJECT_IMPL(Scenario::ProcessModel)
namespace Scenario
{

ProcessModel::ProcessModel(
    const TimeVal& duration, const Id<Process::ProcessModel>& id,
    QObject* parent)
    : Process::ProcessModel{duration, id,
                            Metadata<
                                ObjectKey_k, Scenario::ProcessModel>::get(),
                            parent}
    , inlet{Process::make_inlet(Id<Process::Port>(0), this)}
    , outlet{Process::make_outlet(Id<Process::Port>(0), this)}
    , m_startTimeSyncId{Scenario::startId<TimeSyncModel>()}
    , m_startEventId{Scenario::startId<EventModel>()}
    , m_startStateId{Scenario::startId<StateModel>()}
{
  auto& start_tn = ScenarioCreate<TimeSyncModel>::redo(
      m_startTimeSyncId, {0., 0.1}, TimeVal::zero(), *this);
  start_tn.metadata().setName("Sync.start");
  auto& start_ev = ScenarioCreate<EventModel>::redo(
      m_startEventId, start_tn, {0., 0.0}, *this);
  start_ev.metadata().setName("Event.start");
  auto& start_st = ScenarioCreate<StateModel>::redo(
      m_startStateId, start_ev, 0.02, *this);
  start_st.metadata().setName("State.start");
  // At the end because plug-ins depend on the start/end timesync & al being
  // here
  metadata().setInstanceName(*this);

  inlet->type = Process::PortType::Audio;
  outlet->type = Process::PortType::Audio;
  outlet->setPropagate(true);

  init();
}

void ProcessModel::init()
{
  inlet->setCustomData("In");
  outlet->setCustomData("Out");
  m_inlets.push_back(inlet.get());
  m_outlets.push_back(outlet.get());
}

ProcessModel::~ProcessModel()
{
  try
  {
    score::IDocument::documentContext(*parent()).selectionStack.clear();
  }
  catch (...)
  {
    // Sometimes the scenario isn't in the hierarchy, e.G. in
    // ScenarioPasteElements
  }
  comments.clear();
  intervals.clear();
  states.clear();
  events.clear();
  timeSyncs.clear();

  identified_object_destroying(this);
}

void ProcessModel::setDurationAndScale(const TimeVal& newDuration)
{
  double scale = newDuration / duration();

  for (auto& timesync : timeSyncs)
  {
    timesync.setDate(timesync.date() * scale);
    // Since events will also move we do not need
    // to move the timesync.
  }

  for (auto& event : events)
  {
    event.setDate(event.date() * scale);
    eventMoved(event);
  }
  for (auto& cmt : comments)
  {
    cmt.setDate(cmt.date() * scale);
  }

  for (auto& interval : intervals)
  {
    interval.setStartDate(interval.date() * scale);
    // Note : scale the min / max.

    auto newdur = interval.duration.defaultDuration() * scale;
    IntervalDurations::Algorithms::scaleAllDurations(interval, newdur);

    for (auto& process : interval.processes)
    {
      process.setParentDuration(ExpandMode::Scale, newdur);
    }

    intervalMoved(interval);
  }

  this->setDuration(newDuration);
}

void ProcessModel::setDurationAndGrow(const TimeVal& newDuration)
{
  this->setDuration(newDuration);
}

void ProcessModel::setDurationAndShrink(const TimeVal& newDuration)
{
  this->setDuration(newDuration);
  return; // Disabled by Asana
}

void ProcessModel::startExecution()
{
  // TODO this is called for each process!!
  // But it should be done only once at the global level.
  for (IntervalModel& interval : intervals)
  {
    interval.startExecution();
  }
}

void ProcessModel::stopExecution()
{
  for (IntervalModel& interval : intervals)
  {
    interval.stopExecution();
  }
  for (EventModel& ev : events)
  {
    ev.setStatus(ExecutionStatus::Editing, *this);
  }
}

void ProcessModel::reset()
{
  for (auto& interval : intervals)
  {
    interval.reset();
  }

  for (auto& event : events)
  {
    event.setStatus(Scenario::ExecutionStatus::Editing, *this);
  }
}

Selection ProcessModel::selectableChildren() const
{
  Selection objects;
  apply([&](const auto& m) {
    for (auto& elt : this->*m)
      objects.append(&elt);
  });
  return objects;
}

template <typename InputVec, typename OutputVec>
static void copySelected(const InputVec& in, OutputVec& out)
{
  for (const auto& elt : in)
  {
    if (elt.selection.get())
      out.append(&elt);
  }
}

Selection ProcessModel::selectedChildren() const
{
  Selection objects;
  apply([&](const auto& m) { copySelected(this->*m, objects); });
  return objects;
}

void ProcessModel::changeDuration(IntervalModel& itv, const TimeVal& v)
{
  Command::MoveEventMeta cmd(
      *this, this->states.at(itv.endState()).eventId(), itv.date() + v,
      itv.heightPercentage(), ExpandMode::GrowShrink, LockMode::Free);
  cmd.redo(score::IDocument::documentContext(*this));
}

void ProcessModel::changeDuration(
    const Scenario::IntervalModel& itv, OngoingCommandDispatcher& dispatcher,
    const TimeVal& val, ExpandMode expandmode, LockMode lockmode)
{
  auto& scenario = *this;
  dispatcher.submitCommand<Command::MoveEventMeta>(
      scenario, scenario.state(itv.endState()).eventId(), itv.date() + val,
      itv.heightPercentage(), expandmode, lockmode);
}

void ProcessModel::setSelection(const Selection& s) const
{
  // OPTIMIZEME
  apply([&](auto&& m) {
    for (auto& elt : this->*m)
      elt.selection.set(s.contains(&elt));
  });
}

const QVector<Id<IntervalModel>> intervalsBeforeTimeSync(
    const Scenario::ProcessModel& scenar, const Id<TimeSyncModel>& timeSyncId)
{
  QVector<Id<IntervalModel>> cstrs;
  const auto& tn = scenar.timeSyncs.at(timeSyncId);
  for (const auto& ev : tn.events())
  {
    const auto& evM = scenar.events.at(ev);
    for (const auto& st : evM.states())
    {
      const auto& stM = scenar.states.at(st);
      if (stM.previousInterval())
        cstrs.push_back(*stM.previousInterval());
    }
  }

  return cstrs;
}

const StateModel* furthestSelectedState(const Scenario::ProcessModel& scenar)
{
  const StateModel* furthest{};
  {
    TimeVal max_t = TimeVal::zero();
    double max_y = 0;
    for (StateModel& elt : scenar.states)
    {
      if (elt.selection.get())
      {
        auto date = scenar.events.at(elt.eventId()).date();
        if (!furthest || date > max_t)
        {
          max_t = date;
          max_y = elt.heightPercentage();
          furthest = &elt;
        }
        else if (date == max_t && elt.heightPercentage() > max_y)
        {
          max_y = elt.heightPercentage();
          furthest = &elt;
        }
      }
    }
    if (furthest)
    {
      return furthest;
    }
  }

  // If there is no furthest state, we instead go for a interval
  const IntervalModel* furthest_interval{};
  {
    TimeVal max_t = TimeVal::zero();
    double max_y = 0;
    for (IntervalModel& cst : scenar.intervals)
    {
      if (cst.selection.get())
      {
        auto date = cst.duration.defaultDuration();
        if (!furthest_interval || date > max_t)
        {
          max_t = date;
          max_y = cst.heightPercentage();
          furthest_interval = &cst;
        }
        else if (date == max_t && cst.heightPercentage() > max_y)
        {
          max_y = cst.heightPercentage();
          furthest_interval = &cst;
        }
      }
    }

    if (furthest_interval)
    {
      return &scenar.states.at(furthest_interval->endState());
    }
  }

  return nullptr;
}

const EventModel* furthestSelectedEvent(const Scenario::ProcessModel& scenar)
{
  const EventModel* furthest{};
  {
    TimeVal max_t = TimeVal::zero();
    double max_y = 0;
    for (EventModel& elt : scenar.events)
    {
      if (elt.selection.get())
      {
        auto date = elt.date();
        if (!furthest || date > max_t)
        {
          max_t = date;
          max_y = elt.extent().bottom();
          furthest = &elt;
        }
        else if (date == max_t && elt.extent().bottom() > max_y)
        {
          max_y = elt.extent().bottom();
          furthest = &elt;
        }
      }
    }
  }
  return furthest;
}

const TimeSyncModel* furthestSelectedSync(const Scenario::ProcessModel& scenar)
{
  const TimeSyncModel* furthest{};
  {
    TimeVal max_t = TimeVal::zero();
    double max_y = 0;
    for (TimeSyncModel& elt : scenar.timeSyncs)
    {
      if (elt.selection.get())
      {
        auto date = elt.date();
        if (!furthest || date > max_t)
        {
          max_t = date;
          max_y = elt.extent().bottom();
          furthest = &elt;
        }
        else if (date == max_t && elt.extent().bottom() > max_y)
        {
          max_y = elt.extent().bottom();
          furthest = &elt;
        }
      }
    }
  }
  return furthest;
}

const StateModel* furthestSelectedStateWithoutFollowingInterval(
    const Scenario::ProcessModel& scenar)
{
  const StateModel* furthest_state{};
  {
    TimeVal max_t = TimeVal::zero();
    double max_y = 0;
    for (StateModel& state : scenar.states)
    {
      if (state.selection.get() && !state.nextInterval())
      {
        auto date = scenar.events.at(state.eventId()).date();
        if (!furthest_state || date > max_t)
        {
          max_t = date;
          max_y = state.heightPercentage();
          furthest_state = &state;
        }
        else if (date == max_t && state.heightPercentage() > max_y)
        {
          max_y = state.heightPercentage();
          furthest_state = &state;
        }
      }
    }
    if (furthest_state)
    {
      return furthest_state;
    }
  }

  // If there is no furthest state, we instead go for a interval
  const IntervalModel* furthest_interval{};
  {
    TimeVal max_t = TimeVal::zero();
    double max_y = 0;
    for (IntervalModel& cst : scenar.intervals)
    {
      if (cst.selection.get())
      {
        const auto& state = scenar.states.at(cst.endState());
        if (state.nextInterval())
          continue;

        auto date = cst.duration.defaultDuration();
        if (!furthest_interval || date > max_t)
        {
          max_t = date;
          max_y = cst.heightPercentage();
          furthest_interval = &cst;
        }
        else if (date == max_t && cst.heightPercentage() > max_y)
        {
          max_y = cst.heightPercentage();
          furthest_interval = &cst;
        }
      }
    }

    if (furthest_interval)
    {
      return &scenar.states.at(furthest_interval->endState());
    }
  }

  return nullptr;
}

bool ProcessModel::contentHasDuration() const
{
  return true;
}

TimeVal ProcessModel::contentDuration() const
{
  TimeVal max_tn_pos = TimeVal::zero();
  for (TimeSyncModel& t : timeSyncs)
  {
    if (t.date() > max_tn_pos)
      max_tn_pos = t.date();
  }
  return max_tn_pos;
}

const TimeSyncModel*
furthestHierarchicallySelectedTimeSync(const ProcessModel& scenario)
{
  const Scenario::TimeSyncModel* attach_sync{};

  if (auto furthestState = furthestSelectedState(scenario))
  {
    attach_sync = &Scenario::parentTimeSync(*furthestState, scenario);
  }
  else
  {
    if (auto furthestEvent = furthestSelectedEvent(scenario))
    {
      attach_sync = &Scenario::parentTimeSync(*furthestEvent, scenario);
    }
    else
    {
      attach_sync = furthestSelectedSync(scenario);
    }
  }

  if (!attach_sync)
    attach_sync = &scenario.startTimeSync();

  return attach_sync;
}
}
