// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <Scenario/Document/Synchronization/SynchronizationModel.hpp>
#include <Scenario/Process/Algorithms/StandardCreationPolicy.hpp>
#include <Scenario/Process/Algorithms/VerticalMovePolicy.hpp>

#include <QDataStream>
#include <QtGlobal>
#include <algorithm>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/multi_index/detail/hash_index_iterator.hpp>
#include <score/tools/IdentifierGeneration.hpp>
#include <vector>

#include "SplitSynchronization.hpp"
#include <Scenario/Document/VerticalExtent.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/model/EntityMap.hpp>
#include <score/model/path/Path.hpp>
#include <score/model/path/PathSerialization.hpp>

namespace Scenario
{
namespace Command
{

SplitSynchronization::SplitSynchronization(
    const SynchronizationModel& path, QVector<Id<EventModel>> eventsInNewSynchronization)
    : m_path{path}
    , m_eventsInNewSynchronization(std::move(eventsInNewSynchronization))
{
  m_originalSynchronizationId = path.id();

  auto scenar = static_cast<Scenario::ProcessModel*>(path.parent());
  m_newSynchronizationId = getStrongId(scenar->synchronizations);
}

void SplitSynchronization::undo(const score::DocumentContext& ctx) const
{
  auto& scenar = static_cast<Scenario::ProcessModel&>(*m_path.find(ctx).parent());
  auto& originalTN = scenar.synchronization(m_originalSynchronizationId);
  auto& newTN = scenar.synchronization(m_newSynchronizationId);

  auto events = newTN.events(); // Copy to prevent iterator invalidation
  for (const auto& eventId : events)
  {
    newTN.removeEvent(eventId);
    originalTN.addEvent(eventId);
  }

  ScenarioCreate<SynchronizationModel>::undo(m_newSynchronizationId, scenar);

  updateSynchronizationExtent(originalTN.id(), scenar);
}

void SplitSynchronization::redo(const score::DocumentContext& ctx) const
{
  auto& scenar = static_cast<Scenario::ProcessModel&>(*m_path.find(ctx).parent());
  auto& originalTN = scenar.synchronization(m_originalSynchronizationId);

  // TODO set the correct position here.
  auto& tn = ScenarioCreate<SynchronizationModel>::redo(
      m_newSynchronizationId,
      VerticalExtent{}, // TODO
      originalTN.date(),
      scenar);

  for (const auto& eventId : m_eventsInNewSynchronization)
  {
    originalTN.removeEvent(eventId);
    tn.addEvent(eventId);
  }

  updateSynchronizationExtent(originalTN.id(), scenar);
  updateSynchronizationExtent(tn.id(), scenar);
}

void SplitSynchronization::serializeImpl(DataStreamInput& s) const
{
  s << m_path << m_originalSynchronizationId << m_eventsInNewSynchronization
    << m_newSynchronizationId;
}

void SplitSynchronization::deserializeImpl(DataStreamOutput& s)
{
  s >> m_path >> m_originalSynchronizationId >> m_eventsInNewSynchronization
      >> m_newSynchronizationId;
}
}
}
