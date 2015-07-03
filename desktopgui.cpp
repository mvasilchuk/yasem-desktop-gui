#include "desktopguiplugin.h"
#include "desktopguiobject.h"

using namespace yasem;

DesktopGuiPlugin::DesktopGuiPlugin(QObject *parent):
    SDK::Plugin(parent)
{
    setObjectName("DesktopGUI");
    setMultithreading(false);
}

DesktopGuiPlugin::~DesktopGuiPlugin()
{

}

void DesktopGuiPlugin::register_dependencies()
{
    add_dependency(SDK::PluginDependency(SDK::ROLE_WEB_SERVER, false));
    add_dependency(SDK::PluginDependency(SDK::ROLE_BROWSER, true, true));
    add_dependency(SDK::PluginDependency(SDK::ROLE_WEB_GUI, true, true));
    add_dependency(SDK::PluginDependency(SDK::ROLE_DATASOURCE, true, true));
    add_dependency(SDK::PluginDependency(SDK::ROLE_MEDIA, true, true));
}

void DesktopGuiPlugin::register_roles()
{
    register_role(SDK::ROLE_GUI, new DesktopGuiObject(this));
}
