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
class BaseScenarioTriggerCommandFactory : public TriggerCommandFactory
{
  SCORE_CONCRETE("35ba7a91-c9b1-4ba5-833c-316c0416a828")
public:
  bool matches(const SynchronizationModel& tn) const override;

  score::Command*
  make_addTriggerCommand(const SynchronizationModel& tn) const override;

  score::Command*
  make_removeTriggerCommand(const SynchronizationModel& tn) const override;
};
}
}
