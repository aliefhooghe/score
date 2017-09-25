// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "LoopComponent.hpp"
#include <ossia/editor/state/state_element.hpp>
#include <Engine/LocalTree/Scenario/MetadataParameters.hpp>

namespace Engine
{
namespace LocalTree
{

LoopComponentBase::LoopComponentBase(
    const Id<score::Component>& id,
    ossia::net::node_base& parent,
    Loop::ProcessModel& loop,
    DocumentPlugin& sys,
    QObject* parent_obj)
    : ProcessComponent_T<Loop::ProcessModel>{parent,
                                             loop,
                                             sys,
                                             id,
                                             "LoopComponent",
                                             parent_obj}
    , m_intervalsNode{*node().create_child("intervals")}
    , m_eventsNode{*node().create_child("events")}
    , m_synchronizationsNode{*node().create_child("synchronizations")}
    , m_statesNode{*node().create_child("states")}
{
}

template <>
Interval* LoopComponentBase::make<Interval, Scenario::IntervalModel>(
    const Id<score::Component>& id, Scenario::IntervalModel& elt)
{
  return new Interval{m_intervalsNode, id, elt, system(), this};
}

template <>
Event* LoopComponentBase::make<Event, Scenario::EventModel>(
    const Id<score::Component>& id, Scenario::EventModel& elt)
{
  return new Event{m_eventsNode, id, elt, system(), this};
}

template <>
Synchronization* LoopComponentBase::make<Synchronization, Scenario::SynchronizationModel>(
    const Id<score::Component>& id, Scenario::SynchronizationModel& elt)
{
  return new Synchronization{m_synchronizationsNode, id, elt, system(), this};
}

template <>
State* LoopComponentBase::make<State, Scenario::StateModel>(
    const Id<score::Component>& id, Scenario::StateModel& elt)
{
  return new State{m_statesNode, id, elt, system(), this};
}
}
}
