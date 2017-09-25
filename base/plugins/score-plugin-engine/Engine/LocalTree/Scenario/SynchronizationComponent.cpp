// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "SynchronizationComponent.hpp"
#include "MetadataParameters.hpp"
#include <ossia/editor/state/state_element.hpp>
#include <State/Value.hpp>

namespace Engine
{
namespace LocalTree
{
Synchronization::Synchronization(
    ossia::net::node_base& parent,
    const Id<score::Component>& id,
    Scenario::SynchronizationModel& synchronization,
    DocumentPlugin& doc,
    QObject* parent_comp)
    : CommonComponent{parent, synchronization.metadata(), doc,
                      id,     "StateComponent",    parent_comp}
{
  m_properties.push_back(add_setProperty<::State::impulse>(
      node(), "trigger", [&](auto) { synchronization.triggeredByGui(); }));
}
}
}
