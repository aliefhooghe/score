#pragma once

#include <QVector>
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <score/command/Command.hpp>
#include <score/model/path/Path.hpp>
#include <score/tools/std/Optional.hpp>

#include <score/model/Identifier.hpp>

struct DataStreamInput;
struct DataStreamOutput;

namespace Scenario
{
class EventModel;
class SynchronizationModel;
namespace Command
{
class SplitSynchronization final : public score::Command
{
  SCORE_COMMAND_DECL(
      ScenarioCommandFactoryName(), SplitSynchronization, "Split a sync")
public:
  SplitSynchronization(
      const SynchronizationModel& path, QVector<Id<EventModel>> eventsInNewSynchronization);
  void undo(const score::DocumentContext& ctx) const override;
  void redo(const score::DocumentContext& ctx) const override;

protected:
  void serializeImpl(DataStreamInput&) const override;
  void deserializeImpl(DataStreamOutput&) override;

private:
  Path<SynchronizationModel> m_path;
  QVector<Id<EventModel>> m_eventsInNewSynchronization;

  Id<SynchronizationModel> m_originalSynchronizationId;
  Id<SynchronizationModel> m_newSynchronizationId;
};
}
}
