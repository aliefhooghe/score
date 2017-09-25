#pragma once

#include <Scenario/Commands/Scenario/Displacement/MoveEvent.hpp>
#include <Scenario/Commands/ScenarioCommandFactory.hpp>

#include <Scenario/Document/Synchronization/SynchronizationModel.hpp>
#include <Scenario/Process/Algorithms/GoodOldDisplacementPolicy.hpp>
#include <Scenario/Process/Algorithms/StandardDisplacementPolicy.hpp>
#include <Scenario/Process/Algorithms/VerticalMovePolicy.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <State/Expression.hpp>
#include <score/model/tree/TreeNode.hpp>

#include <score/command/Command.hpp>

#include <score/model/path/Path.hpp>
#include <score/model/path/PathSerialization.hpp>
#include <score/model/Identifier.hpp>
//#include <Scenario/Application/ScenarioValidity.hpp>
#include <score/serialization/DataStreamVisitor.hpp>

namespace Scenario
{

namespace Command
{
class SCORE_PLUGIN_SCENARIO_EXPORT MergeSynchronizations final
    : public score::Command
{
    SCORE_COMMAND_DECL(ScenarioCommandFactoryName(), MergeSynchronizations, "Merge Synchronizations")
public:
  MergeSynchronizations(
      const ProcessModel& scenario,
      Id<SynchronizationModel>
          clickedTn,
      Id<SynchronizationModel>
          hoveredTn)
      : m_scenarioPath{scenario}
      , m_movingTnId{std::move(clickedTn)}
      , m_destinationTnId{std::move(hoveredTn)}
  {
    auto& tn = scenario.synchronization(m_movingTnId);
    auto& destinantionTn = scenario.synchronization(m_destinationTnId);

    QByteArray arr;
    DataStream::Serializer s{&arr};
    s.readFrom(tn);
    m_serializedSynchronization = arr;

    m_moveCommand = new MoveEvent<GoodOldDisplacementPolicy>{
        scenario, tn.events().front(),
        destinantionTn.date(), ExpandMode::Scale, LockMode::Free};

    m_targetTrigger = destinantionTn.expression();
    m_targetTriggerActive = destinantionTn.active();
  }

  void undo(const score::DocumentContext& ctx) const override
  {
    auto& scenar = m_scenarioPath.find(ctx);

    //ScenarioValidityChecker::checkValidity(scenar);
    auto& globalTn = scenar.synchronization(m_destinationTnId);

    DataStream::Deserializer s{m_serializedSynchronization};
    auto recreatedTn = new SynchronizationModel{s, &scenar};

    auto events_in_timesync = recreatedTn->events();
    // we remove and re-add events in recreated Tn
    // to ensure correct parentship between elements.
    for (auto evId : events_in_timesync)
    {
      recreatedTn->removeEvent(evId);
      globalTn.removeEvent(evId);
    }
    for (auto evId : events_in_timesync)
    {
      recreatedTn->addEvent(evId);
    }

    scenar.synchronizations.add(recreatedTn);

    globalTn.setExpression(m_targetTrigger);
    globalTn.setActive(m_targetTriggerActive);

    //ScenarioValidityChecker::checkValidity(scenar);
    m_moveCommand->undo(ctx);
    updateSynchronizationExtent(m_destinationTnId, scenar);

    //ScenarioValidityChecker::checkValidity(scenar);
  }
  void redo(const score::DocumentContext& ctx) const override
  {
    auto& scenar = m_scenarioPath.find(ctx);
    //ScenarioValidityChecker::checkValidity(scenar);
    m_moveCommand->redo(ctx);
    //ScenarioValidityChecker::checkValidity(scenar);

    auto& movingTn = scenar.synchronization(m_movingTnId);
    auto& destinationTn = scenar.synchronization(m_destinationTnId);

    auto movingEvents = movingTn.events();
    for (auto& evId : movingEvents)
    {
      movingTn.removeEvent(evId);
      destinationTn.addEvent(evId);
    }
    destinationTn.setActive(
        destinationTn.active() || movingTn.active());
    destinationTn.setExpression(movingTn.expression());

    scenar.synchronizations.remove(m_movingTnId);
    updateSynchronizationExtent(m_destinationTnId, scenar);
    //ScenarioValidityChecker::checkValidity(scenar);
  }

  void update(
      unused_t scenar,
      const Id<SynchronizationModel>& clickedTn,
      const Id<SynchronizationModel>& hoveredTn)
  {
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_scenarioPath << m_movingTnId << m_destinationTnId
      << m_serializedSynchronization << m_moveCommand->serialize() << m_targetTrigger
      << m_targetTriggerActive;
  }

  void deserializeImpl(DataStreamOutput& s) override
  {
    QByteArray cmd;

    s >> m_scenarioPath >> m_movingTnId >> m_destinationTnId
        >> m_serializedSynchronization >> cmd >> m_targetTrigger
        >> m_targetTriggerActive;

    m_moveCommand = new MoveEvent<GoodOldDisplacementPolicy>{};
    m_moveCommand->deserialize(cmd);
  }

private:
  Path<ProcessModel> m_scenarioPath;
  Id<SynchronizationModel> m_movingTnId;
  Id<SynchronizationModel> m_destinationTnId;

  QByteArray m_serializedSynchronization;
  MoveEvent<GoodOldDisplacementPolicy>* m_moveCommand{};
  State::Expression m_targetTrigger;
  bool m_targetTriggerActive{};
};
}
}
