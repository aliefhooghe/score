// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QByteArray>
#include <Scenario/Commands/Synchronization/AddTrigger.hpp>
#include <Scenario/Commands/Synchronization/RemoveTrigger.hpp>
#include <Scenario/Document/BaseScenario/BaseScenario.hpp>
#include <Scenario/Document/Synchronization/SynchronizationModel.hpp>

#include "BaseScenarioTriggerCommandFactory.hpp"
#include <Scenario/Process/ScenarioInterface.hpp>
#include <score/command/Command.hpp>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/model/path/Path.hpp>
#include <score/model/path/PathSerialization.hpp>

namespace Scenario
{
namespace Command
{
bool BaseScenarioTriggerCommandFactory::matches(const SynchronizationModel& tn) const
{
  return dynamic_cast<BaseScenario*>(tn.parent());
}

score::Command*
BaseScenarioTriggerCommandFactory::make_addTriggerCommand(
    const SynchronizationModel& tn) const
{
  if (dynamic_cast<BaseScenario*>(tn.parent()))
  {
    return new Scenario::Command::AddTrigger<BaseScenario>(tn);
  }
  return nullptr;
}

score::Command*
BaseScenarioTriggerCommandFactory::make_removeTriggerCommand(
    const SynchronizationModel& tn) const
{
  if (dynamic_cast<BaseScenario*>(tn.parent()))
  {
    return new Scenario::Command::RemoveTrigger<BaseScenario>(tn);
  }
  return nullptr;
}
}
}
