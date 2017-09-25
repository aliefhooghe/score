// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/editor/state/state_element.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <ossia/editor/state/state.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <Engine/score2OSSIA.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Scenario/Document/BaseScenario/BaseScenario.hpp>
#include <algorithm>
#include <score/document/DocumentInterface.hpp>
#include <vector>

#include "BaseScenarioComponent.hpp"
#include <Engine/Executor/IntervalComponent.hpp>
#include <Engine/Executor/EventComponent.hpp>
#include <Engine/Executor/StateComponent.hpp>
#include <Engine/Executor/SynchronizationComponent.hpp>
#include <Scenario/Document/Interval/IntervalDurations.hpp>
#include <Scenario/Document/Interval/IntervalModel.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/Synchronization/SynchronizationModel.hpp>
#include <score/tools/IdentifierGeneration.hpp>
#include <Engine/Executor/ExecutorContext.hpp>

#include <Engine/OSSIA2score.hpp>

namespace Engine
{
namespace Execution
{
BaseScenarioElement::BaseScenarioElement(
    const Context& ctx,
    QObject* parent)
    : QObject{nullptr}, m_ctx{ctx}
{
}

BaseScenarioElement::~BaseScenarioElement()
{
}

void BaseScenarioElement::init(BaseScenarioRefContainer element)
{
  auto main_start_node = std::make_shared<ossia::time_sync>();
  auto main_end_node = std::make_shared<ossia::time_sync>();

  auto main_start_event = *main_start_node->emplace(
      main_start_node->get_time_events().begin(),
      [this](auto&&...) {},
      ossia::expressions::make_expression_true());
  auto main_end_event = *main_end_node->emplace(
      main_end_node->get_time_events().begin(),
      [this](auto&&...) {},
      ossia::expressions::make_expression_true());

  // TODO PlayDuration of base interval.
  // TODO PlayDuration of FullView
  auto main_interval = ossia::time_interval::create(
      [](auto&&...) {},
      *main_start_event,
      *main_end_event,
      m_ctx.time(element.interval().duration.defaultDuration()),
      m_ctx.time(element.interval().duration.minDuration()),
      m_ctx.time(element.interval().duration.maxDuration()));

  m_ossia_startSynchronization = std::make_shared<SynchronizationComponent>(
      element.startSynchronization(), m_ctx, newId(element.startSynchronization()), this);
  m_ossia_endSynchronization = std::make_shared<SynchronizationComponent>(
      element.endSynchronization(), m_ctx, newId(element.endSynchronization()), this);

  m_ossia_startEvent = std::make_shared<EventComponent>(element.startEvent(), m_ctx, newId(element.startEvent()), this);
  m_ossia_endEvent = std::make_shared<EventComponent>(element.endEvent(), m_ctx, newId(element.endEvent()), this);

  m_ossia_startState = std::make_shared<StateComponent>(element.startState(), m_ctx, newId(element.startState()), this);
  m_ossia_endState =  std::make_shared<StateComponent>(element.endState(), m_ctx, newId(element.endState()), this);

  m_ossia_interval = std::make_shared<IntervalComponent>(
      element.interval(), m_ctx, newId(element.interval()), this);

  m_ossia_startSynchronization->onSetup(main_start_node, m_ossia_startSynchronization->makeTrigger());
  m_ossia_endSynchronization->onSetup(main_end_node, m_ossia_endSynchronization->makeTrigger());
  m_ossia_startEvent->onSetup(main_start_event, m_ossia_startEvent->makeExpression(), (ossia::time_event::offset_behavior)element.startEvent().offsetBehavior());
  m_ossia_endEvent->onSetup(main_end_event, m_ossia_endEvent->makeExpression(), (ossia::time_event::offset_behavior)element.endEvent().offsetBehavior());
  m_ossia_startState->onSetup(main_start_event);
  m_ossia_endState->onSetup(main_end_event);
  m_ossia_interval->onSetup(main_interval, m_ossia_interval->makeDurations(), true);

}

void BaseScenarioElement::cleanup()
{
  if(m_ossia_interval)
    m_ossia_interval->cleanup();
  if(m_ossia_startState)
    m_ossia_startState->cleanup();
  if(m_ossia_endState)
    m_ossia_endState->cleanup();
  if(m_ossia_startEvent)
    m_ossia_startEvent->cleanup();
  if(m_ossia_endEvent)
    m_ossia_endEvent->cleanup();
  if(m_ossia_startSynchronization)
  {
    m_ossia_startSynchronization->OSSIASynchronization()->cleanup();
    m_ossia_startSynchronization->cleanup();
  }
  if(m_ossia_endSynchronization)
  {
    m_ossia_endSynchronization->OSSIASynchronization()->cleanup();
    m_ossia_endSynchronization->cleanup();
  }

  m_ossia_interval.reset();
  m_ossia_startState.reset();
  m_ossia_endState.reset();
  m_ossia_startEvent.reset();
  m_ossia_endEvent.reset();
  m_ossia_startSynchronization.reset();
  m_ossia_endSynchronization.reset();
}

IntervalComponent& BaseScenarioElement::baseInterval() const
{
  return *m_ossia_interval;
}

SynchronizationComponent& BaseScenarioElement::startSynchronization() const
{
  return *m_ossia_startSynchronization;
}

SynchronizationComponent& BaseScenarioElement::endSynchronization() const
{
  return *m_ossia_endSynchronization;
}

EventComponent& BaseScenarioElement::startEvent() const
{
  return *m_ossia_startEvent;
}

EventComponent& BaseScenarioElement::endEvent() const
{
  return *m_ossia_endEvent;
}

StateComponent& BaseScenarioElement::startState() const
{
  return *m_ossia_startState;
}

StateComponent& BaseScenarioElement::endState() const
{
  return *m_ossia_endState;
}
}
}

BaseScenarioRefContainer::BaseScenarioRefContainer(
    Scenario::IntervalModel& interval, Scenario::ScenarioInterface& s)
    : m_interval{interval}
    , m_startState{s.state(interval.startState())}
    , m_endState{s.state(interval.endState())}
    , m_startEvent{s.event(m_startState.eventId())}
    , m_endEvent{s.event(m_endState.eventId())}
    , m_startNode{s.synchronization(m_startEvent.synchronization())}
    , m_endNode{s.synchronization(m_endEvent.synchronization())}
{
}
