#include "desktopgui.h"
#include "pluginmanager.h"
#include "mainwindow.h"
#include "profilemanager.h"

#include <QApplication>
#include <QMenu>

using namespace yasem;

DesktopGUI::DesktopGUI(): GuiPlugin()
{
    setObjectName("DesktopGUI");
}

QRect DesktopGUI::getWindowRect()
{
   return this->mainWindow->geometry();
}

void DesktopGUI::setWindowRect(const QRect &rect)
{
    MainWindow* wnd = this->mainWindow;
    qDebug() << rect;
    wnd->setGeometry(rect);
}

void DesktopGUI::setFullscreen(bool fullscreen)
{
    this->mainWindow->setAppFullscreen(fullscreen);
}

bool DesktopGUI::getFullscreen()
{
    return this->mainWindow->isFullScreen();
}

void DesktopGUI::repaintGui()
{
    this->mainWindow->resizeWebView();
}

PLUGIN_ERROR_CODES DesktopGUI::initialize()
{
    STUB();

    DEBUG() << "========== Starting desktop GUI ==========";

    ProfileManager::instance()->loadProfiles();

    mainWindow = new MainWindow();
    mainWindow->initialize();
    mainWindow->setAttribute(Qt::WA_QuitOnClose);
    mainWindow->show();

    return PLUGIN_ERROR_NO_ERROR;
}

PLUGIN_ERROR_CODES DesktopGUI::deinitialize()
{
    DEBUG() << "deinitialize()";
    return PLUGIN_ERROR_NO_ERROR;
}

void DesktopGUI::register_dependencies()
{
    add_dependency(PluginDependency(ROLE_WEB_SERVER, false));
    add_dependency(PluginDependency(ROLE_BROWSER, true, true));
    add_dependency(PluginDependency(ROLE_WEB_GUI, true, true));
    add_dependency(PluginDependency(ROLE_DATASOURCE, true, true));
    add_dependency(PluginDependency(ROLE_MEDIA, true, true));
}

void DesktopGUI::register_roles()
{
    register_role(ROLE_GUI);
    register_role(ROLE_STB_API_SYSTEM);
}

QList<QMenu*> DesktopGUI::getMenuItems()
{
    return mainWindow->getMenuItems();
}

QRect DesktopGUI::widgetRect()
{
    return mainWindow->windowInternalRect();
}
