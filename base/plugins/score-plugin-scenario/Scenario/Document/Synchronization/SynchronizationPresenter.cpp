// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <Scenario/Document/Synchronization/SynchronizationModel.hpp>
#include <Scenario/Document/Synchronization/SynchronizationView.hpp>
#include <Scenario/Document/Synchronization/TriggerView.hpp>
#include <score/widgets/GraphicsItem.hpp>

#include "SynchronizationPresenter.hpp"
#include <score/model/ModelMetadata.hpp>
#include <score/selection/Selectable.hpp>

#include <score/tools/Todo.hpp>

class QObject;
#include <score/model/Identifier.hpp>

namespace Scenario
{
SynchronizationPresenter::SynchronizationPresenter(
    const SynchronizationModel& model,
        QGraphicsItem* parentview,
        QObject* parent)
    : QObject{parent}
    , m_model{model}
    , m_view{new SynchronizationView{*this, parentview}}
    , m_triggerView{new TriggerView{m_view}}
{
  con(m_model.selection, &Selectable::changed, this,
      [=](bool b) { m_view->setSelected(b); });

  con(m_model, &SynchronizationModel::newEvent, this,
      &SynchronizationPresenter::on_eventAdded);

  con(m_model.metadata(), &score::ModelMetadata::ColorChanged, this,
      [=](const score::ColorRef& c) { m_view->changeColor(c); });
  con(m_model.metadata(), &score::ModelMetadata::LabelChanged, this,
      [=](const auto& t) { m_view->setLabel(t); });
  con(m_model, &SynchronizationModel::activeChanged, this, [=] {
    m_view->setTriggerActive(m_model.active());
    m_triggerView->setVisible(m_model.active());
    m_triggerView->setToolTip(m_model.expression().toString());
  });
  m_view->changeColor(m_model.metadata().getColor());
  m_view->setLabel(m_model.metadata().getLabel());
  m_view->setTriggerActive(m_model.active());
  // TODO find a correct way to handle validity of model elements.
  // extentChanged is updated in scenario.

  m_triggerView->setVisible(m_model.active());
  m_triggerView->setPos(-7.5, -25.);

  m_triggerView->setToolTip(m_model.expression().toString());
  con(m_model, &SynchronizationModel::triggerChanged, this,
      [&](const State::Expression& t) { m_triggerView->setToolTip(t.toString()); });

  connect(
      m_triggerView, &TriggerView::pressed,
              &m_model, [=] (QPointF sp) {
      m_model.triggeredByGui();
      pressed(sp);
  });
}

SynchronizationPresenter::~SynchronizationPresenter()
{
}

const Id<SynchronizationModel>& SynchronizationPresenter::id() const
{
  return m_model.id();
}

const SynchronizationModel& SynchronizationPresenter::model() const
{
  return m_model;
}

SynchronizationView* SynchronizationPresenter::view() const
{
  return m_view;
}

void SynchronizationPresenter::on_eventAdded(const Id<EventModel>& eventId)
{
  emit eventAdded(eventId, m_model.id());
}
}
