#ifndef DESKTOPGUIOBJECT_H
#define DESKTOPGUIOBJECT_H

#include "gui.h"

#include <QObject>

namespace yasem {

class MainWindow;

namespace SDK {
class Profile;
class Plugin;
}

class DesktopGuiObject: public SDK::GUI
{
public:
    DesktopGuiObject(SDK::Plugin* plugin);
    virtual ~DesktopGuiObject();

    // GuiPlugin interface
public:
    QRect getWindowRect();
    void setWindowRect(const QRect &rect);
    void setFullscreen(bool fullscreen);
    bool getFullscreen();
    void repaintGui();
    QList<QMenu *> getMenuItems();
    QRect widgetRect();
    QMainWindow *window();

protected:
    MainWindow* m_main_window;
    bool m_fullscreen;
    SDK::Profile* m_config_profile;

    // AbstractPluginObject interface
public:
    SDK::PluginObjectResult init();
    SDK::PluginObjectResult deinit();

};

}

#endif // DESKTOPGUIOBJECT_H
