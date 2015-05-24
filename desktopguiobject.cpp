#include "desktopguiobject.h"
#include "pluginmanager.h"
#include "mainwindow.h"
#include "profilemanager.h"
#include "macros.h"

#include <QRect>


using namespace yasem;

DesktopGuiObject::DesktopGuiObject(Plugin* plugin):
    GuiPluginObject(plugin)
{

}

DesktopGuiObject::~DesktopGuiObject()
{

}

QRect DesktopGuiObject::getWindowRect()
{
    return this->m_main_window->geometry();
}

void DesktopGuiObject::setWindowRect(const QRect &rect)
{
    MainWindow* wnd = this->m_main_window;
    DEBUG() << rect;
    wnd->setGeometry(rect);
}

void DesktopGuiObject::setFullscreen(bool fullscreen)
{
    this->m_main_window->setAppFullscreen(fullscreen);
}

bool DesktopGuiObject::getFullscreen()
{
    return this->m_main_window->isFullScreen();
}

void DesktopGuiObject::repaintGui()
{
    this->m_main_window->resizeWebView();
}

QList<QMenu *> DesktopGuiObject::getMenuItems()
{
    return m_main_window->getMenuItems();
}

QRect DesktopGuiObject::widgetRect()
{
    return m_main_window->windowInternalRect();
}

PluginObjectResult DesktopGuiObject::init()
{
    ProfileManager::instance()->loadProfiles();

    setInitialized(true);

    m_main_window = new MainWindow();
    m_main_window->initialize();
    m_main_window->setAttribute(Qt::WA_QuitOnClose);
    m_main_window->show();

    return PLUGIN_OBJECT_RESULT_OK;
}

PluginObjectResult DesktopGuiObject::deinit()
{
    return PLUGIN_OBJECT_RESULT_OK;
}
