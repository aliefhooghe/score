#pragma once
#include <Scenario/Commands/MoveBaseEvent.hpp>
#include <Scenario/Document/BaseScenario/BaseScenario.hpp>
#include <Scenario/Palette/Tools/SmartTool.hpp>
#include <Scenario/Palette/Tools/States/MoveAndMergeState.hpp>
#include <Scenario/Palette/Tools/States/MoveStates.hpp>
#include <Scenario/Palette/Transitions/IntervalTransitions.hpp>
#include <Scenario/Palette/Transitions/EventTransitions.hpp>
#include <Scenario/Palette/Transitions/StateTransitions.hpp>
#include <Scenario/Palette/Transitions/SynchronizationTransitions.hpp>

namespace Scenario
{
class MoveIntervalInBaseScenario_StateWrapper
{
public:
  template <typename Scenario_T, typename ToolPalette_T>
  static void make(const ToolPalette_T& palette, QState* waitState, QState& sm)
  {
    // We cannot move the interval
  }
};

class MoveEventInBaseScenario_StateWrapper
{
public:
  template <typename Scenario_T, typename ToolPalette_T>
  static void make(const ToolPalette_T& palette, QState* waitState, QState& sm)
  {
    /// Event
    auto moveEvent = new Scenario::
        MoveEventState<Command::MoveBaseEvent<Scenario_T>, Scenario_T, ToolPalette_T>{
            palette, palette.model(), palette.context().context.commandStack,
            palette.context().context.objectLocker, &sm};

    score::make_transition<Scenario::ClickOnEndState_Transition<Scenario_T>>(
        waitState, moveEvent, *moveEvent);

    score::make_transition<Scenario::ClickOnEndEvent_Transition<Scenario_T>>(
        waitState, moveEvent, *moveEvent);
    moveEvent->addTransition(moveEvent, finishedState(), waitState);
  }
};

class MoveSynchronizationInBaseScenario_StateWrapper
{
public:
  template <typename Scenario_T, typename ToolPalette_T>
  static void make(const ToolPalette_T& palette, QState* waitState, QState& sm)
  {
    /// Synchronization
    auto moveSynchronization = new Scenario::
        MoveSynchronizationState<Command::MoveBaseEvent<Scenario_T>, Scenario_T, ToolPalette_T>{
            palette, palette.model(), palette.context().context.commandStack,
            palette.context().context.objectLocker, &sm};

    score::
        make_transition<Scenario::ClickOnEndSynchronization_Transition<Scenario_T>>(
            waitState, moveSynchronization, *moveSynchronization);
    moveSynchronization->addTransition(moveSynchronization, finishedState(), waitState);
  }
};
}
