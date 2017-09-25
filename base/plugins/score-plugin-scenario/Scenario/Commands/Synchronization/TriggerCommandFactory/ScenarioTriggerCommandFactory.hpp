#pragma once
#include <Scenario/Commands/Synchronization/TriggerCommandFactory/TriggerCommandFactory.hpp>

namespace score
{
class Command;
} // namespace score
namespace Scenario
{
class SynchronizationModel;
namespace Command
{

class ScenarioTriggerCommandFactory : public TriggerCommandFactory
{
  SCORE_CONCRETE("26e38b07-13fa-4f6d-9b95-1bdaeeafab9e")
public:
  bool matches(const SynchronizationModel& tn) const override;

  score::Command*
  make_addTriggerCommand(const SynchronizationModel& tn) const override;

  score::Command*
  make_removeTriggerCommand(const SynchronizationModel& tn) const override;
};
}
}
