// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <Scenario/Process/Algorithms/StandardCreationPolicy.hpp>
#include <Scenario/Process/ScenarioModel.hpp>

#include <QByteArray>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/multi_index/detail/hash_index_iterator.hpp>
#include <score/tools/IdentifierGeneration.hpp>
#include <vector>

#include "CreateSynchronization_Event_State.hpp"
#include <Process/TimeValue.hpp>
#include <Scenario/Commands/Scenario/Creations/CreateEvent_State.hpp>
#include <Scenario/Document/Synchronization/SynchronizationModel.hpp>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/model/EntityMap.hpp>

namespace Scenario
{
namespace Command
{
CreateSynchronization_Event_State::CreateSynchronization_Event_State(
    const Scenario::ProcessModel& scenario,
    TimeVal date,
    double stateY)
    : m_newSynchronization{getStrongId(scenario.synchronizations)}
    , m_date{std::move(date)}
    , m_command{scenario, m_newSynchronization, stateY}
{
}

void CreateSynchronization_Event_State::undo(const score::DocumentContext& ctx) const
{
  m_command.undo(ctx);

  ScenarioCreate<SynchronizationModel>::undo(
      m_newSynchronization, m_command.scenarioPath().find(ctx));
}

void CreateSynchronization_Event_State::redo(const score::DocumentContext& ctx) const
{
  auto& scenar = m_command.scenarioPath().find(ctx);

  // Create the node
  ScenarioCreate<SynchronizationModel>::redo(
      m_newSynchronization, {0.4, 0.6}, m_date, scenar);

  // And the event
  m_command.redo(ctx);
}

void CreateSynchronization_Event_State::serializeImpl(DataStreamInput& s) const
{
  s << m_newSynchronization << m_date << m_command.serialize();
}

void CreateSynchronization_Event_State::deserializeImpl(DataStreamOutput& s)
{
  QByteArray b;
  s >> m_newSynchronization >> m_date >> b;

  m_command.deserialize(b);
}
}
}
