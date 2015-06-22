#include "desktopguiplugin.h"
#include "desktopguiobject.h"

using namespace yasem;

DesktopGuiPlugin::DesktopGuiPlugin(QObject *parent):
    Plugin(parent)
{
    setObjectName("DesktopGUI");
    setMultithreading(false);
}

DesktopGuiPlugin::~DesktopGuiPlugin()
{

}

void DesktopGuiPlugin::register_dependencies()
{
    add_dependency(PluginDependency(ROLE_WEB_SERVER, false));
    add_dependency(PluginDependency(ROLE_BROWSER, true, true));
    add_dependency(PluginDependency(ROLE_WEB_GUI, true, true));
    add_dependency(PluginDependency(ROLE_DATASOURCE, true, true));
    add_dependency(PluginDependency(ROLE_MEDIA, true, true));
}

void DesktopGuiPlugin::register_roles()
{
    register_role(ROLE_GUI, new DesktopGuiObject(this));
}
