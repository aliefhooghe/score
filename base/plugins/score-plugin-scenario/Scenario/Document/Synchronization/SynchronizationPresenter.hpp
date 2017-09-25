#pragma once

#include <QPoint>
#include <score/model/Identifier.hpp>
#include <score/widgets/GraphicsItem.hpp>
#include <score_plugin_scenario_export.h>
#include <sys/types.h>

class QGraphicsItem;
class QObject;

namespace Scenario
{
class EventModel;
class SynchronizationModel;
class SynchronizationView;
class TriggerView;

class SCORE_PLUGIN_SCENARIO_EXPORT SynchronizationPresenter final : public QObject
{
  Q_OBJECT
public:
  SynchronizationPresenter(
      const SynchronizationModel& model, QGraphicsItem* parentview, QObject* parent);
  ~SynchronizationPresenter();

  const Id<SynchronizationModel>& id() const;
  int32_t id_val() const
  {
    return id().val();
  }

  const SynchronizationModel& model() const;
  SynchronizationView* view() const;

  void on_eventAdded(const Id<EventModel>& eventId);

signals:
  void pressed(const QPointF&);
  void moved(const QPointF&);
  void released(const QPointF&);

  void eventAdded(
      const Id<EventModel>& eventId, const Id<SynchronizationModel>& synchronizationId);

private:
  const SynchronizationModel& m_model;
  graphics_item_ptr<SynchronizationView> m_view;
  TriggerView* m_triggerView{};
};
}
