#include <NetworkApplicationPlugin.hpp>
#include <iscore_plugin_network.hpp>

#include "DistributedScenario/Commands/DistributedScenarioCommandFactory.hpp"
#include "DistributedScenario/Panel/GroupPanelFactory.hpp"
#include "iscore/plugins/qt_interfaces/GUIApplicationContextPlugin_QtInterface.hpp"
#include <iscore_plugin_network_commands_files.hpp>

namespace iscore {
class Application;
class PanelFactory;
}  // namespace iscore

#define PROCESS_NAME "Network Process"

iscore_plugin_network::iscore_plugin_network() :
    QObject {},
        iscore::GUIApplicationContextPlugin_QtInterface {}//,
        //iscore::SettingsDelegateFactoryInterface_QtInterface {}
{
}

// Interfaces implementations :
//////////////////////////
/*
iscore::SettingsDelegateFactoryInterface* iscore_plugin_network::settings_make()
{
    return new NetworkSettings;
}
*/
iscore::GUIApplicationContextPlugin* iscore_plugin_network::make_applicationPlugin(iscore::Application& app)
{
    return new NetworkApplicationPlugin{app};
}

std::vector<iscore::PanelFactory*> iscore_plugin_network::panels()
{
    return {new GroupPanelFactory};
}


#include <iscore/command/CommandGeneratorMap.hpp>
#include <unordered_map>

std::pair<const CommandParentFactoryKey, CommandGeneratorMap> iscore_plugin_network::make_commands()
{
    std::pair<const CommandParentFactoryKey, CommandGeneratorMap> cmds{DistributedScenarioCommandFactoryName(), CommandGeneratorMap{}};

    using Types = iscore::commands::TypeList<
#include <iscore_plugin_network_commands.hpp>
      >;
    iscore::commands::ForEach<Types>(iscore::commands::FactoryInserter{cmds.second});


    return cmds;
}
