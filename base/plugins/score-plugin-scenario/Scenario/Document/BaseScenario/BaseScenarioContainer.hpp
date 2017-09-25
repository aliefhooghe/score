#pragma once
#include <Scenario/Process/ScenarioInterface.hpp>
#include <tuple>

#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>
#include <score/model/Identifier.hpp>
#include <score/tools/std/IndirectContainer.hpp>
#include <score_plugin_scenario_export.h>

class DataStream;

class JSONObject;
class QObject;

namespace Scenario
{
class IntervalModel;
class EventModel;
class StateModel;
class SynchronizationModel;
class SCORE_PLUGIN_SCENARIO_EXPORT BaseScenarioContainer
    : public ScenarioInterface
{
  SCORE_SERIALIZE_FRIENDS
public:
  struct no_init
  {
  };
  explicit BaseScenarioContainer(QObject* parentObject);
  explicit BaseScenarioContainer(no_init, QObject* parentObject);
  explicit BaseScenarioContainer(
      const BaseScenarioContainer&, QObject* parentObject);

  virtual ~BaseScenarioContainer();

  QObject& parentObject() const
  {
    return *m_parent;
  }

  ElementContainer<IntervalModel> getIntervals() const final override
  {
    return {m_interval};
  }
  ElementContainer<StateModel> getStates() const final override
  {
    return {m_startState, m_endState};
  }
  ElementContainer<EventModel> getEvents() const final override
  {
    return {m_startEvent, m_endEvent};
  }
  ElementContainer<SynchronizationModel> getSynchronizations() const final override
  {
    return {m_startNode, m_endNode};
  }

  IntervalModel*
  findInterval(const Id<IntervalModel>& id) const final override;

  EventModel* findEvent(const Id<EventModel>& id) const final override;

  SynchronizationModel*
  findSynchronization(const Id<SynchronizationModel>& id) const final override;

  StateModel* findState(const Id<StateModel>& id) const final override;

  IntervalModel&
  interval(const Id<IntervalModel>& id) const final override;

  EventModel& event(const Id<EventModel>& id) const final override;

  SynchronizationModel& synchronization(const Id<SynchronizationModel>& id) const final override;

  StateModel& state(const Id<StateModel>& id) const final override;

  IntervalModel& interval() const;

  SynchronizationModel& startSynchronization() const final override;
  SynchronizationModel& endSynchronization() const;

  EventModel& startEvent() const;
  EventModel& endEvent() const;

  StateModel& startState() const;
  StateModel& endState() const;

  score::IndirectArray<IntervalModel, 1> intervals() const
  {
    return m_interval;
  }
  score::IndirectArray<EventModel, 2> events() const
  {
    return {m_startEvent, m_endEvent};
  }
  score::IndirectArray<StateModel, 2> states() const
  {
    return {m_startState, m_endState};
  }
  score::IndirectArray<SynchronizationModel, 2> synchronizations() const
  {
    return {m_startNode, m_endNode};
  }

protected:
  SynchronizationModel* m_startNode{};
  SynchronizationModel* m_endNode{};

  EventModel* m_startEvent{};
  EventModel* m_endEvent{};

  StateModel* m_startState{};
  StateModel* m_endState{};

  IntervalModel* m_interval{};

  auto elements() const
  {
    return std::make_tuple(
        m_startNode, m_endNode, m_startEvent, m_endEvent, m_startState,
        m_endState, m_interval);
  }

private:
  QObject* m_parent{}; // Parent for the intervals, timesyncs, etc.
                       // If inheriting, m_parent should be this.
};

inline auto intervals(const BaseScenarioContainer& scenar)
{
  return scenar.intervals();
}
inline auto events(const BaseScenarioContainer& scenar)
{
  return scenar.events();
}
inline auto synchronizations(const BaseScenarioContainer& scenar)
{
  return scenar.synchronizations();
}
inline auto states(const BaseScenarioContainer& scenar)
{
  return scenar.states();
}

template <>
struct ElementTraits<BaseScenarioContainer, IntervalModel>
{
  static const constexpr auto accessor
      = static_cast<score::IndirectArray<IntervalModel, 1> (*)(
          const BaseScenarioContainer&)>(&intervals);
};
template <>
struct ElementTraits<BaseScenarioContainer, EventModel>
{
  static const constexpr auto accessor
      = static_cast<score::IndirectArray<EventModel, 2> (*)(
          const BaseScenarioContainer&)>(&events);
};
template <>
struct ElementTraits<BaseScenarioContainer, SynchronizationModel>
{
  static const constexpr auto accessor
      = static_cast<score::IndirectArray<SynchronizationModel, 2> (*)(
          const BaseScenarioContainer&)>(&synchronizations);
};
template <>
struct ElementTraits<BaseScenarioContainer, StateModel>
{
  static const constexpr auto accessor
      = static_cast<score::IndirectArray<StateModel, 2> (*)(
          const BaseScenarioContainer&)>(&states);
};
}
