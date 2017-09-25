#pragma once

#include <Inspector/InspectorWidgetBase.hpp>
#include <Process/TimeValue.hpp>
#include <vector>

namespace Inspector
{
class InspectorSectionWidget;
}
class QLabel;
namespace Scenario
{
class MetadataWidget;
class TriggerInspectorWidget;
class EventModel;
class SynchronizationModel;
/*!
 * \brief The SynchronizationInspectorWidget class
 *      Inherits from InspectorWidgetInterface. Manages an inteface for an
 * Synchronization (Timebox) element.
 */
class SynchronizationInspectorWidget final : public Inspector::InspectorWidgetBase
{
public:
  explicit SynchronizationInspectorWidget(
      const SynchronizationModel& object,
      const score::DocumentContext& context,
      QWidget* parent);

private:
  QString tabName() override;

  void updateDisplayedValues();
  void on_dateChanged(const TimeVal&);

  const SynchronizationModel& m_model;

  MetadataWidget* m_metadata{};
  QLabel* m_date{};
  TriggerInspectorWidget* m_trigwidg{};
};
}
