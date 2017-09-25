#pragma once
#include <Process/TimeValue.hpp>
#include <score/command/Command.hpp>
#include <score/tools/std/Optional.hpp>

#include "CreateEvent_State.hpp"
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <score/model/path/Path.hpp>
#include <score/model/Identifier.hpp>
#include <score_plugin_scenario_export.h>
struct DataStreamInput;
struct DataStreamOutput;
namespace Scenario
{
class ProcessModel;
class EventModel;
class StateModel;
class SynchronizationModel;
namespace Command
{
class SCORE_PLUGIN_SCENARIO_EXPORT CreateSynchronization_Event_State final
    : public score::Command
{
  SCORE_COMMAND_DECL(
      ScenarioCommandFactoryName(),
      CreateSynchronization_Event_State,
      "Create a timesync, an event and a state")
public:
  CreateSynchronization_Event_State(
      const Scenario::ProcessModel& scenario, TimeVal date, double stateY);

  const Path<Scenario::ProcessModel>& scenarioPath() const
  {
    return m_command.scenarioPath();
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
  TimeVal m_date;

  CreateEvent_State m_command;
};
}
}
