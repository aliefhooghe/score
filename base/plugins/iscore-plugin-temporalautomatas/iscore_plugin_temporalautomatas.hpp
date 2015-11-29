#pragma once
#include <iscore/plugins/qt_interfaces/GUIApplicationContextPlugin_QtInterface.hpp>
#include <qobject.h>

#include "iscore/plugins/application/GUIApplicationContextPlugin.hpp"

namespace iscore {
class Application;
}  // namespace iscore

namespace TemporalAutomatas
{
}
class iscore_plugin_temporalautomatas final:
        public QObject,
        public iscore::GUIApplicationContextPlugin_QtInterface
{
        Q_OBJECT
        Q_PLUGIN_METADATA(IID GUIApplicationContextPlugin_QtInterface_iid)
        Q_INTERFACES(
                iscore::GUIApplicationContextPlugin_QtInterface
                )

    public:
        iscore_plugin_temporalautomatas();
        virtual ~iscore_plugin_temporalautomatas() = default;

        iscore::GUIApplicationContextPlugin* make_applicationPlugin(
                iscore::Application& app);

};
