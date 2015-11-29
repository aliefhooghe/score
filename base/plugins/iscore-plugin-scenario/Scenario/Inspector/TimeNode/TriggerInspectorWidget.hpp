#pragma once

#include <qwidget.h>

#include "State/Expression.hpp"

class InspectorWidgetBase;
class QPushButton;
class SimpleExpressionEditorWidget;
class TimeNodeModel;
class TriggerCommandFactoryList;

class TriggerInspectorWidget final : public QWidget
{
    public:
        TriggerInspectorWidget(
                const TriggerCommandFactoryList& fact,
                const TimeNodeModel& object,
                InspectorWidgetBase* parent);

        void on_triggerChanged();

        void createTrigger();
        void removeTrigger();

        void on_triggerActiveChanged();
        void HideRmButton();

        void updateExpression(const iscore::Trigger&);

    private:
        const TriggerCommandFactoryList& m_triggerCommandFactory;
        const TimeNodeModel& m_model;

        InspectorWidgetBase* m_parent;

        QPushButton* m_addTrigBtn{};
        QPushButton* m_rmTrigBtn{};

        SimpleExpressionEditorWidget* m_exprEditor{};
};
