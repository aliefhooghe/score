// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <Scenario/Process/Algorithms/StandardCreationPolicy.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <score/tools/RandomNameProvider.hpp>

#include <QByteArray>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/multi_index/detail/hash_index_iterator.hpp>
#include <score/tools/IdentifierGeneration.hpp>
#include <vector>

#include "CreateInterval_State_Event_Synchronization.hpp"
#include <Process/TimeValue.hpp>
#include <Scenario/Commands/Scenario/Creations/CreateInterval_State_Event.hpp>
#include <Scenario/Document/Synchronization/SynchronizationModel.hpp>
#include <score/model/ModelMetadata.hpp>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/model/EntityMap.hpp>

namespace Scenario
{
namespace Command
{
CreateInterval_State_Event_Synchronization::CreateInterval_State_Event_Synchronization(
    const Scenario::ProcessModel& scenario,
    Id<StateModel>
        startState,
    TimeVal date,
    double endStateY)
    : m_newSynchronization{getStrongId(scenario.synchronizations)}
    , m_createdName{RandomNameProvider::generateRandomName()}
    , m_command{scenario, std::move(startState), m_newSynchronization, endStateY}
    , m_date{std::move(date)}
{
}

void CreateInterval_State_Event_Synchronization::undo(const score::DocumentContext& ctx) const
{
  m_command.undo(ctx);

  ScenarioCreate<SynchronizationModel>::undo(
      m_newSynchronization, m_command.scenarioPath().find(ctx));
}

void CreateInterval_State_Event_Synchronization::redo(const score::DocumentContext& ctx) const
{
  auto& scenar = m_command.scenarioPath().find(ctx);

  // Create the end timesync
  ScenarioCreate<SynchronizationModel>::redo(
      m_newSynchronization,
      {m_command.endStateY(), m_command.endStateY()},
      m_date,
      scenar);

  scenar.synchronization(m_newSynchronization).metadata().setName(m_createdName);

  // The event + state + interval between
  m_command.redo(ctx);
}

void CreateInterval_State_Event_Synchronization::serializeImpl(
    DataStreamInput& s) const
{
  s << m_newSynchronization << m_createdName << m_command.serialize() << m_date;
}

void CreateInterval_State_Event_Synchronization::deserializeImpl(
    DataStreamOutput& s)
{
  QByteArray b;
  s >> m_newSynchronization >> m_createdName >> b >> m_date;

  m_command.deserialize(b);
}
}
}
