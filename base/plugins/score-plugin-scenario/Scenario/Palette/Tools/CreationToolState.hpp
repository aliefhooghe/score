#pragma once
#include "ScenarioToolState.hpp"
#include <Scenario/Palette/ScenarioPaletteBaseStates.hpp>

#include <Scenario/Palette/Transitions/EventTransitions.hpp>
#include <Scenario/Palette/Transitions/NothingTransitions.hpp>
#include <Scenario/Palette/Transitions/StateTransitions.hpp>
#include <Scenario/Palette/Transitions/SynchronizationTransitions.hpp>

#include <Scenario/Palette/Tools/States/ScenarioCreation_FromEvent.hpp>
#include <Scenario/Palette/Tools/States/ScenarioCreation_FromNothing.hpp>
#include <Scenario/Palette/Tools/States/ScenarioCreation_FromState.hpp>
#include <Scenario/Palette/Tools/States/ScenarioCreation_FromSynchronization.hpp>

#include <Scenario/Palette/Tools/ScenarioToolState.hpp>

#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/Event/EventPresenter.hpp>
#include <Scenario/Document/Event/EventView.hpp>
#include <Scenario/Document/Synchronization/SynchronizationModel.hpp>
#include <Scenario/Document/Synchronization/SynchronizationPresenter.hpp>
#include <Scenario/Document/Synchronization/SynchronizationView.hpp>

#include <score/document/DocumentInterface.hpp>
#include <score/statemachine/StateMachineTools.hpp>

class EventPresenter;
class SynchronizationPresenter;
namespace Scenario
{

template <typename Scenario_T, typename ToolPalette_T>
class CreationTool final : public ToolBase<ToolPalette_T>
{
public:
  CreationTool(ToolPalette_T& sm) : ToolBase<ToolPalette_T>{sm}
  {
    m_waitState = new QState;
    this->localSM().addState(m_waitState);
    this->localSM().setInitialState(m_waitState);

    //// Create from nothing ////
    m_createFromNothingState
        = new Creation_FromNothing<Scenario_T, ToolPalette_T>{
            this->m_palette, this->m_palette.model(),
            this->m_palette.context().context.commandStack, nullptr};

    score::make_transition<ClickOnNothing_Transition<Scenario_T>>(
        m_waitState, m_createFromNothingState, *m_createFromNothingState);
    m_createFromNothingState->addTransition(
        m_createFromNothingState, finishedState(), m_waitState);

    this->localSM().addState(m_createFromNothingState);

    //// Create from an event ////
    m_createFromEventState = new Creation_FromEvent<Scenario_T, ToolPalette_T>{
        this->m_palette, this->m_palette.model(),
        this->m_palette.context().context.commandStack, nullptr};

    score::make_transition<ClickOnEvent_Transition<Scenario_T>>(
        m_waitState, m_createFromEventState, *m_createFromEventState);
    m_createFromEventState->addTransition(
        m_createFromEventState, finishedState(), m_waitState);

    this->localSM().addState(m_createFromEventState);

    //// Create from a timesync ////
    m_createFromSynchronizationState
        = new Creation_FromSynchronization<Scenario_T, ToolPalette_T>{
            this->m_palette, this->m_palette.model(),
            this->m_palette.context().context.commandStack, nullptr};

    score::make_transition<ClickOnSynchronization_Transition<Scenario_T>>(
        m_waitState, m_createFromSynchronizationState, *m_createFromSynchronizationState);
    m_createFromSynchronizationState->addTransition(
        m_createFromSynchronizationState, finishedState(), m_waitState);

    this->localSM().addState(m_createFromSynchronizationState);

    //// Create from a State ////
    m_createFromStateState = new Creation_FromState<Scenario_T, ToolPalette_T>{
        this->m_palette, this->m_palette.model(),
        this->m_palette.context().context.commandStack, nullptr};

    score::make_transition<ClickOnState_Transition<Scenario_T>>(
        m_waitState, m_createFromStateState, *m_createFromStateState);

    m_createFromStateState->addTransition(
        m_createFromStateState, finishedState(), m_waitState);

    this->localSM().addState(m_createFromStateState);

    this->localSM().start();
  }

  void on_pressed(QPointF scene, Scenario::Point sp)
  {
    this->mapTopItem(
        this->itemUnderMouse(scene),

        // Press a state
        [&](const Id<StateModel>& id) {
          this->localSM().postEvent(new ClickOnState_Event{id, sp});
        },

        // Press an event
        [&](const Id<EventModel>& id) {
          this->localSM().postEvent(new ClickOnEvent_Event{id, sp});
        },

        // Press a Synchronization
        [&](const Id<SynchronizationModel>& id) {
          this->localSM().postEvent(new ClickOnSynchronization_Event{id, sp});
        },

        // Press a Interval
        [&](const Id<IntervalModel>&) {},
        // Press a Brace (left)
        [&](const Id<IntervalModel>&) {},
        // (right)
        [&](const Id<IntervalModel>&) {},

        // Press a slot handle
        [&](const SlotPath&) {},

        // Click on the background
        [&]() {

          // Here we have the logic for the creation in nothing
          // where we instead choose the latest state if selected
          if (auto state = furthestSelectedState(this->m_palette.model()))
          {
            if (this->m_palette.model().events.at(state->eventId()).date()
                < sp.date)
            {
              this->localSM().postEvent(
                  new ClickOnState_Event{state->id(), sp});
              return;
            }
          }

          this->localSM().postEvent(new ClickOnNothing_Event{sp});

        });
  }
  void on_moved(QPointF scene, Scenario::Point sp)
  {
    if (auto cs = currentState())
    {
      mapWithCollision(
          scene,
          [&](const Id<StateModel>& id) {
            this->localSM().postEvent(new MoveOnState_Event{id, sp});
          },
          [&](const Id<EventModel>& id) {
            this->localSM().postEvent(new MoveOnEvent_Event{id, sp});
          },
          [&](const Id<SynchronizationModel>& id) {
            this->localSM().postEvent(new MoveOnSynchronization_Event{id, sp});
          },
          [&]() { this->localSM().postEvent(new MoveOnNothing_Event{sp}); },
          cs->createdStates,
          cs->createdEvents,
          cs->createdSynchronizations);
    }
  }
  void on_released(QPointF scene, Scenario::Point sp)
  {
    if (auto cs = currentState())
    {
      mapWithCollision(
          scene,
          [&](const Id<StateModel>& id) {
            this->localSM().postEvent(new ReleaseOnState_Event{id, sp});
          },
          [&](const Id<EventModel>& id) {
            this->localSM().postEvent(new ReleaseOnEvent_Event{id, sp});
          },
          [&](const Id<SynchronizationModel>& id) {
            this->localSM().postEvent(new ReleaseOnSynchronization_Event{id, sp});
          },
          [&]() { this->localSM().postEvent(new ReleaseOnNothing_Event{sp}); },
          cs->createdStates,
          cs->createdEvents,
          cs->createdSynchronizations);
    }
  }

private:
  // Return the colliding elements that were not created in the current
  // commands
  QList<Id<StateModel>> getCollidingStates(
      QPointF point, const QVector<Id<StateModel>>& createdStates)
  {
    return getCollidingModels(
        this->m_palette.presenter().getStates(), createdStates, point);
  }
  QList<Id<EventModel>> getCollidingEvents(
      QPointF point, const QVector<Id<EventModel>>& createdEvents)
  {
    return getCollidingModels(
        this->m_palette.presenter().getEvents(), createdEvents, point);
  }
  QList<Id<SynchronizationModel>> getCollidingSynchronizations(
      QPointF point, const QVector<Id<SynchronizationModel>>& createdSynchronizations)
  {
    return getCollidingModels(
        this->m_palette.presenter().getSynchronizations(), createdSynchronizations, point);
  }

  CreationState<Scenario_T, ToolPalette_T>* currentState() const
  {
    if (isStateActive(m_createFromEventState))
      return m_createFromEventState;
    else if (isStateActive(m_createFromNothingState))
      return m_createFromNothingState;
    else if (isStateActive(m_createFromStateState))
      return m_createFromStateState;
    else if (isStateActive(m_createFromSynchronizationState))
      return m_createFromSynchronizationState;
    else
      return nullptr;
  }

  template <
      typename StateFun,
      typename EventFun,
      typename SynchronizationFun,
      typename NothingFun>
  void mapWithCollision(
      QPointF point,
      StateFun st_fun,
      EventFun ev_fun,
      SynchronizationFun tn_fun,
      NothingFun nothing_fun,
      const QVector<Id<StateModel>>& createdStates,
      const QVector<Id<EventModel>>& createdEvents,
      const QVector<Id<SynchronizationModel>>& createdSynchronizations)
  {
    auto collidingStates = getCollidingStates(point, createdStates);
    if (!collidingStates.empty())
    {
      st_fun(collidingStates.first());
      return;
    }

    auto collidingEvents = getCollidingEvents(point, createdEvents);
    if (!collidingEvents.empty())
    {
      ev_fun(collidingEvents.first());
      return;
    }

    auto collidingSynchronizations = getCollidingSynchronizations(point, createdSynchronizations);
    if (!collidingSynchronizations.empty())
    {
      tn_fun(collidingSynchronizations.first());
      return;
    }

    nothing_fun();
  }

  Creation_FromNothing<Scenario_T, ToolPalette_T>* m_createFromNothingState{};
  Creation_FromEvent<Scenario_T, ToolPalette_T>* m_createFromEventState{};
  Creation_FromSynchronization<Scenario_T, ToolPalette_T>*
      m_createFromSynchronizationState{};
  Creation_FromState<Scenario_T, ToolPalette_T>* m_createFromStateState{};
  QState* m_waitState{};
};
}
