// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QByteArray>
#include <QMap>
#include <QPair>
#include <score/tools/std/Optional.hpp>

#include "GoodOldDisplacementPolicy.hpp"
#include <Scenario/Document/Interval/IntervalDurations.hpp>
#include <Scenario/Document/Interval/IntervalModel.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/Synchronization/SynchronizationModel.hpp>
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <Scenario/Tools/dataStructures.hpp>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/model/EntityMap.hpp>
#include <score/model/path/Path.hpp>
#include <score/model/Identifier.hpp>

template <typename T>
class Reader;

namespace Scenario
{
void GoodOldDisplacementPolicy::computeDisplacement(
    Scenario::ProcessModel& scenario,
    const QVector<Id<SynchronizationModel>>& draggedElements,
    const TimeVal& deltaTime,
    ElementsProperties& elementsProperties)
{
  // this old behavior supports only the move of one timesync
  if (draggedElements.length() != 1)
  {
    qDebug()
        << "WARNING : computeDisplacement called with empty element list !";
    // move nothing, nothing to undo or redo
    return;
  }
  else
  {
    const Id<SynchronizationModel>& firstSynchronizationMovedId = draggedElements.at(0);
    std::vector<Id<SynchronizationModel>> synchronizationsToTranslate;

    GoodOldDisplacementPolicy::getRelatedSynchronizations(
        scenario, firstSynchronizationMovedId, synchronizationsToTranslate);

    // put each concerned timesync in modified elements and compute new values
    for (const auto& curSynchronizationId : synchronizationsToTranslate)
    {
      auto& curSynchronization = scenario.synchronizations.at(curSynchronizationId);

      // if timesync NOT already in element properties, create new element
      // properties and set the old date
      auto tn_it = elementsProperties.timesyncs.find(curSynchronizationId);
      if (tn_it == elementsProperties.timesyncs.end())
      {
        TimenodeProperties t;
        t.oldDate = curSynchronization.date();
        tn_it = elementsProperties.timesyncs.emplace(curSynchronizationId, std::move(t)).first;
      }

      // put the new date
      auto& val = tn_it.value();
      val.newDate = val.oldDate + deltaTime;
    }

    // Make a list of the intervals that need to be resized
    for (const auto& curSynchronizationId : synchronizationsToTranslate)
    {
      auto& curSynchronization = scenario.synchronization(curSynchronizationId);

      // each previous interval
      for (const auto& ev_id : curSynchronization.events())
      {
        const auto& ev = scenario.event(ev_id);
        for (const auto& st_id : ev.states())
        {
          const auto& st = scenario.states.at(st_id);
          if (const auto& optCurIntervalId = st.previousInterval())
          {
            auto curIntervalId = *optCurIntervalId;
            auto& curInterval = scenario.intervals.at(curIntervalId);

            // if timesync NOT already in element properties, create new
            // element properties and set old values
            auto cur_interval_it
                = elementsProperties.intervals.find(curIntervalId);
            if (cur_interval_it == elementsProperties.intervals.end())
            {
              IntervalProperties c{curInterval};
              c.oldMin = curInterval.duration.minDuration();
              c.oldMax = curInterval.duration.maxDuration();

              cur_interval_it
                  = elementsProperties.intervals.emplace(curIntervalId, std::move(c)).first;
            }

            auto& curIntervalStartEvent
                = Scenario::startEvent(curInterval, scenario);
            auto& startTnodeId = curIntervalStartEvent.synchronization();

            // compute default duration
            TimeVal startDate;

            // if prev tnode has moved take updated value else take existing
            auto it = elementsProperties.timesyncs.find(startTnodeId);
            if (it != elementsProperties.timesyncs.cend())
            {
              startDate = it.value().newDate;
            }
            else
            {
              startDate = curIntervalStartEvent.date();
            }

            const auto& endDate
                = elementsProperties.timesyncs[curSynchronizationId].newDate;

            TimeVal newDefaultDuration = endDate - startDate;
            TimeVal deltaBounds = newDefaultDuration
                                    - curInterval.duration.defaultDuration();

            auto& val = cur_interval_it.value();
            val.newMin = curInterval.duration.minDuration() + deltaBounds;
            val.newMax = curInterval.duration.maxDuration() + deltaBounds;

            // nothing to do for now
          }
        }
      }
    }
  }
}

void GoodOldDisplacementPolicy::getRelatedSynchronizations(
    Scenario::ProcessModel& scenario,
    const Id<SynchronizationModel>& firstSynchronizationMovedId,
    std::vector<Id<SynchronizationModel>>& translatedSynchronizations)
{
  if (firstSynchronizationMovedId.val() == Scenario::startId_val())
    return;

  auto it = std::find(
      translatedSynchronizations.begin(),
      translatedSynchronizations.end(),
      firstSynchronizationMovedId);
  if (it == translatedSynchronizations.end())
  {
    translatedSynchronizations.push_back(firstSynchronizationMovedId);
  }
  else // synchronization already moved
  {
    return;
  }

  const auto& cur_synchronization = scenario.synchronizations.at(firstSynchronizationMovedId);
  for (const auto& cur_eventId : cur_synchronization.events())
  {
    const auto& cur_event = scenario.events.at(cur_eventId);

    for (const auto& state_id : cur_event.states())
    {
      const auto& state = scenario.states.at(state_id);
      if (const auto& cons = state.nextInterval())
      {
        const auto& endStateId = scenario.intervals.at(*cons).endState();
        const auto& endTnId
            = scenario.events.at(scenario.state(endStateId).eventId())
                  .synchronization();
        getRelatedSynchronizations(scenario, endTnId, translatedSynchronizations);
      }
    }
  }
}
}
