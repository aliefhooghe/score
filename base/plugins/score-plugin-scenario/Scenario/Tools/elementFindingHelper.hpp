#pragma once

#include <Process/TimeValue.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
namespace Scenario
{
inline TimeVal
getDate(const Scenario::ProcessModel& scenario, const Id<StateModel>& state)
{
  return scenario.synchronizations
      .at(scenario.events.at(scenario.states.at(state).eventId()).synchronization())
      .date();
}

inline TimeVal
getDate(const Scenario::ProcessModel& scenario, const Id<EventModel>& event)
{
  return scenario.synchronizations.at(scenario.events.at(event).synchronization()).date();
}

inline TimeVal getDate(
    const Scenario::ProcessModel& scenario, const Id<SynchronizationModel>& timesync)
{
  return scenario.synchronizations.at(timesync).date();
}
}
