#pragma once
#include <Scenario/Commands/Interval/SetMaxDuration.hpp>
#include <Scenario/Commands/Interval/SetMinDuration.hpp>
#include <Scenario/Commands/Scenario/Displacement/MoveInterval.hpp>
#include <Scenario/Commands/Scenario/Displacement/MoveEventMeta.hpp>
#include <Scenario/Palette/Tools/States/MoveAndMergeState.hpp>
#include <Scenario/Palette/Tools/States/MoveStates.hpp>

#include <Scenario/Palette/Transitions/IntervalTransitions.hpp>
#include <Scenario/Palette/Transitions/EventTransitions.hpp>
#include <Scenario/Palette/Transitions/StateTransitions.hpp>
#include <Scenario/Palette/Transitions/SynchronizationTransitions.hpp>
namespace Scenario
{
class MoveIntervalInScenario_StateWrapper
{
public:
  template <typename Scenario_T, typename ToolPalette_T>
  static void
  make(const ToolPalette_T& palette, QState* waitState, QState& parent)
  {
    /// Interval
    /// //TODO remove useless arguments to ctor
    auto moveInterval
        = new MoveIntervalState<ToolPalette_T>{
            palette, palette.model(), palette.context().context.commandStack,
            palette.context().context.objectLocker, &parent};

    score::make_transition<ClickOnInterval_Transition<Scenario_T>>(
        waitState, moveInterval, *moveInterval);
    moveInterval->addTransition(moveInterval, finishedState(), waitState);
  }
};

class MoveLeftBraceInScenario_StateWrapper
{
public:
  template <typename Scenario_T, typename ToolPalette_T>
  static void
  make(const ToolPalette_T& palette, QState* waitState, QState& parent)
  {
    auto moveBrace
        = new MoveIntervalBraceState<Scenario::Command::SetMinDuration, Scenario_T, ToolPalette_T>{
            palette, palette.model(), palette.context().context.commandStack,
            palette.context().context.objectLocker, &parent};
    score::make_transition<ClickOnLeftBrace_Transition<Scenario_T>>(
        waitState, moveBrace, *moveBrace);
    moveBrace->addTransition(moveBrace, finishedState(), waitState);
  }
};

class MoveRightBraceInScenario_StateWrapper
{
public:
  template <typename Scenario_T, typename ToolPalette_T>
  static void
  make(const ToolPalette_T& palette, QState* waitState, QState& parent)
  {
    auto moveBrace
        = new MoveIntervalBraceState<Scenario::Command::SetMaxDuration, Scenario_T, ToolPalette_T>{
            palette, palette.model(), palette.context().context.commandStack,
            palette.context().context.objectLocker, &parent};
    score::make_transition<ClickOnRightBrace_Transition<Scenario_T>>(
        waitState, moveBrace, *moveBrace);
    moveBrace->addTransition(moveBrace, finishedState(), waitState);
  }
};

class MoveEventInScenario_StateWrapper
{
public:
  template <typename Scenario_T, typename ToolPalette_T>
  static void
  make(const ToolPalette_T& palette, QState* waitState, QState& parent)
  {
    /// Event
    auto moveEvent
        = new MoveEventState<Scenario::Command::MoveEventMeta, Scenario_T, ToolPalette_T>{
            palette, palette.model(), palette.context().context.commandStack,
            palette.context().context.objectLocker, &parent};

    score::make_transition<ClickOnState_Transition<Scenario_T>>(
        waitState, moveEvent, *moveEvent);

    score::make_transition<ClickOnEvent_Transition<Scenario_T>>(
        waitState, moveEvent, *moveEvent);
    moveEvent->addTransition(moveEvent, finishedState(), waitState);
  }
};

class MoveSynchronizationInScenario_StateWrapper
{
public:
  template <typename Scenario_T, typename ToolPalette_T>
  static void
  make(const ToolPalette_T& palette, QState* waitState, QState& parent)
  {
    /// Synchronization
    auto moveSynchronization
        = new MoveSynchronizationState<Scenario::Command::MoveEventMeta, Scenario_T, ToolPalette_T>{
            palette, palette.model(), palette.context().context.commandStack,
            palette.context().context.objectLocker, &parent};

    score::make_transition<ClickOnSynchronization_Transition<Scenario_T>>(
        waitState, moveSynchronization, *moveSynchronization);
    moveSynchronization->addTransition(moveSynchronization, finishedState(), waitState);
  }
};

class MoveEventInTopScenario_StateWrapper
{
public:
  template <typename Scenario_T, typename ToolPalette_T>
  static void
  make(const ToolPalette_T& palette, QState* waitState, QState& parent)
  {
    /// Event
    auto moveEvent
        = new MoveEventState<Scenario::Command::MoveTopEventMeta, Scenario_T, ToolPalette_T>{
            palette, palette.model(), palette.context().context.commandStack,
            palette.context().context.objectLocker, &parent};

    score::make_transition<ClickOnState_Transition<Scenario_T>>(
        waitState, moveEvent, *moveEvent);

    score::make_transition<ClickOnEvent_Transition<Scenario_T>>(
        waitState, moveEvent, *moveEvent);
    moveEvent->addTransition(moveEvent, finishedState(), waitState);
  }
};

class MoveSynchronizationInTopScenario_StateWrapper
{
public:
  template <typename Scenario_T, typename ToolPalette_T>
  static void
  make(const ToolPalette_T& palette, QState* waitState, QState& parent)
  {
    /// Synchronization
    auto moveSynchronization
        = new MoveSynchronizationState<Scenario::Command::MoveTopEventMeta, Scenario_T, ToolPalette_T>{
            palette, palette.model(), palette.context().context.commandStack,
            palette.context().context.objectLocker, &parent};

    score::make_transition<ClickOnSynchronization_Transition<Scenario_T>>(
        waitState, moveSynchronization, *moveSynchronization);
    moveSynchronization->addTransition(moveSynchronization, finishedState(), waitState);
  }
};
}
