// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <Loop/LoopProcessModel.hpp>
#include <QByteArray>
#include <Scenario/Commands/Synchronization/AddTrigger.hpp>
#include <Scenario/Commands/Synchronization/RemoveTrigger.hpp>
#include <Scenario/Document/Synchronization/SynchronizationModel.hpp>

#include "LoopTriggerCommandFactory.hpp"
#include <Scenario/Process/ScenarioInterface.hpp>
#include <score/command/Command.hpp>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/model/path/Path.hpp>
#include <score/model/path/PathSerialization.hpp>

bool LoopTriggerCommandFactory::matches(
    const Scenario::SynchronizationModel& tn) const
{
  return dynamic_cast<Loop::ProcessModel*>(tn.parent());
}

score::Command* LoopTriggerCommandFactory::make_addTriggerCommand(
    const Scenario::SynchronizationModel& tn) const
{
  if (dynamic_cast<Loop::ProcessModel*>(tn.parent()))
  {
    return new Scenario::Command::AddTrigger<Loop::ProcessModel>(tn);
  }
  return nullptr;
}

score::Command*
LoopTriggerCommandFactory::make_removeTriggerCommand(
    const Scenario::SynchronizationModel& tn) const
{
  if (dynamic_cast<Loop::ProcessModel*>(tn.parent()))
  {
    return new Scenario::Command::RemoveTrigger<Loop::ProcessModel>(tn);
  }
  return nullptr;
}
