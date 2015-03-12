#ifndef DESKTOPGUIOBJECT_H
#define DESKTOPGUIOBJECT_H

#include "guipluginobject.h"

#include <QObject>

namespace yasem {

class MainWindow;
class Profile;
class Plugin;

class DesktopGuiObject: public GuiPluginObject
{
public:
    DesktopGuiObject(Plugin* plugin);
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

protected:
    MainWindow* m_main_window;
    bool m_fullscreen;
    Profile* m_config_profile;

    // AbstractPluginObject interface
public:
    PluginObjectResult init();
    PluginObjectResult deinit();
};

}

#endif // DESKTOPGUIOBJECT_H
