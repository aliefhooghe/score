#pragma once
#include <Process/TimeValue.hpp>
#include <QString>
#include <score/command/Command.hpp>
#include <score/tools/std/Optional.hpp>

#include "CreateInterval_State_Event.hpp"
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <score/model/path/Path.hpp>
#include <score/model/Identifier.hpp>
#include <score_plugin_scenario_export.h>
struct DataStreamInput;
struct DataStreamOutput;

namespace Scenario
{
class ProcessModel;
class IntervalModel;
class EventModel;
class StateModel;
class SynchronizationModel;
namespace Command
{
class SCORE_PLUGIN_SCENARIO_EXPORT CreateInterval_State_Event_Synchronization final
    : public score::Command
{
  SCORE_COMMAND_DECL(
      ScenarioCommandFactoryName(),
      CreateInterval_State_Event_Synchronization,
      "Create a interval, a state, an event and a sync")
public:
  CreateInterval_State_Event_Synchronization(
      const Scenario::ProcessModel& scenario,
      Id<StateModel>
          startState,
      TimeVal date,
      double endStateY);

  const Path<Scenario::ProcessModel>& scenarioPath() const
  {
    return m_command.scenarioPath();
  }

  const Id<IntervalModel>& createdInterval() const
  {
    return m_command.createdInterval();
  }

  const Id<StateModel>& startState() const
  {
    return m_command.startState();
  }

  const Id<StateModel>& createdState() const
  {
    return m_command.createdState();
  }

  const Id<EventModel>& createdEvent() const
  {
    return m_command.createdEvent();
  }

  const Id<SynchronizationModel>& createdSynchronization() const
  {
    return m_newSynchronization;
  }

  void undo(const score::DocumentContext& ctx) const override;
  void redo(const score::DocumentContext& ctx) const override;

protected:
  void serializeImpl(DataStreamInput&) const override;
  void deserializeImpl(DataStreamOutput&) override;

private:
  Id<SynchronizationModel> m_newSynchronization;
  QString m_createdName;

  CreateInterval_State_Event m_command;

  TimeVal m_date;
};
}
}
