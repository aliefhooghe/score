#pragma once
#include <Scenario/Palette/ScenarioPaletteBaseTransitions.hpp>
#include <Scenario/Process/ScenarioInterface.hpp>
namespace Scenario
{
template <typename Scenario_T>
class Transition_T<Scenario_T, ClickOnSynchronization> final
    : public MatchedTransition<Scenario_T, ClickOnSynchronization_Event>
{
public:
  using MatchedTransition<Scenario_T, ClickOnSynchronization_Event>::
      MatchedTransition;

protected:
  void onTransition(QEvent* ev) override
  {
    auto qev = static_cast<ClickOnSynchronization_Event*>(ev);
    this->state().clear();

    this->state().clickedSynchronization = qev->id;
    this->state().currentPoint = qev->point;
  }
};
template <typename Scenario_T>
using ClickOnSynchronization_Transition = Transition_T<Scenario_T, ClickOnSynchronization>;

template <typename Scenario_T>
class ClickOnEndSynchronization_Transition final
    : public MatchedTransition<Scenario_T, ClickOnSynchronization_Event>
{
public:
  using MatchedTransition<Scenario_T, ClickOnSynchronization_Event>::
      MatchedTransition;

protected:
  bool eventTest(QEvent* e) override
  {
    if (e->type()
        == QEvent::Type(QEvent::User + ClickOnSynchronization_Event::user_type))
    {
      auto qev = static_cast<ClickOnSynchronization_Event*>(e);
      return qev->id == Scenario::endId<SynchronizationModel>();
    }
    return false;
  }

  void onTransition(QEvent* ev) override
  {
    auto qev = static_cast<ClickOnSynchronization_Event*>(ev);
    this->state().clear();

    this->state().clickedSynchronization = qev->id;
    this->state().currentPoint = qev->point;
  }
};

template <typename Scenario_T>
class Transition_T<Scenario_T, MoveOnSynchronization> final
    : public MatchedTransition<Scenario_T, MoveOnSynchronization_Event>
{
public:
  using MatchedTransition<Scenario_T, MoveOnSynchronization_Event>::MatchedTransition;

protected:
  void onTransition(QEvent* ev) override
  {
    auto qev = static_cast<MoveOnSynchronization_Event*>(ev);

    this->state().hoveredSynchronization = qev->id;
    this->state().currentPoint = qev->point;
  }
};
template <typename Scenario_T>
using MoveOnSynchronization_Transition = Transition_T<Scenario_T, MoveOnSynchronization>;

template <typename Scenario_T>
class Transition_T<Scenario_T, ReleaseOnSynchronization> final
    : public MatchedTransition<Scenario_T, ReleaseOnSynchronization_Event>
{
public:
  using MatchedTransition<Scenario_T, ReleaseOnSynchronization_Event>::
      MatchedTransition;

protected:
  void onTransition(QEvent* ev) override
  {
    auto qev = static_cast<ReleaseOnSynchronization_Event*>(ev);

    this->state().hoveredSynchronization = qev->id;
    this->state().currentPoint = qev->point;
  }
};
template <typename Scenario_T>
using ReleaseOnSynchronization_Transition
    = Transition_T<Scenario_T, ReleaseOnSynchronization>;
}
