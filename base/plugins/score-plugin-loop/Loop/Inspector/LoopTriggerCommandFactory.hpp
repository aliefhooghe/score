#pragma once
#include <Scenario/Commands/Synchronization/TriggerCommandFactory/TriggerCommandFactory.hpp>

namespace Scenario
{
class SynchronizationModel;
}
namespace score
{
class Command;
} // namespace score

class LoopTriggerCommandFactory
    : public Scenario::Command::TriggerCommandFactory
{
  SCORE_CONCRETE("dd32c40b-bf76-4f6c-a8e4-25132ec61d93")
public:
  bool matches(const Scenario::SynchronizationModel& tn) const override;

  score::Command*
  make_addTriggerCommand(const Scenario::SynchronizationModel& tn) const override;

  score::Command*
  make_removeTriggerCommand(const Scenario::SynchronizationModel& tn) const override;
};
