#include <qstring.h>

#include "AutomationStateInspector.hpp"
#include "AutomationStateInspectorFactory.hpp"
#include "Inspector/InspectorWidgetFactoryInterface.hpp"
#include "Automation/State/AutomationState.hpp"
class InspectorWidgetBase;
class QObject;
class QWidget;
namespace iscore {
class Document;
}  // namespace iscore

AutomationStateInspectorFactory::AutomationStateInspectorFactory() :
    InspectorWidgetFactory {}
{

}

InspectorWidgetBase* AutomationStateInspectorFactory::makeWidget(
        const QObject& sourceElement,
        iscore::Document& doc,
        QWidget* parent)
{
    return new AutomationStateInspector{
                safe_cast<const AutomationState&>(sourceElement),
                doc,
                parent};
}

const QList<QString>&AutomationStateInspectorFactory::key_impl() const
{
    static const QList<QString> lst{"AutomationState"};
    return lst;
}
