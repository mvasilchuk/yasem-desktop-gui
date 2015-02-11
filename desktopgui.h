#ifndef DESKTOPGUI_H
#define DESKTOPGUI_H

#include "desktopgui_global.h"
#include "guiplugin.h"
#include "mediaplayerplugin.h"
#include "datasourceplugin.h"
#include "stbplugin.h"

class QMenu;

namespace yasem
{

class MainWindow;
class DESKTOPGUISHARED_EXPORT DesktopGUI: public QObject, public GuiPlugin, public Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.mvas.yasem.DesktopGuiPlugin/1.0" FILE "metadata.json")
    Q_INTERFACES(yasem::Plugin yasem::GuiPlugin )

    Q_CLASSINFO("author", "Maxim Vasilchuk")
    Q_CLASSINFO("description", "Desktop GUI for YASEM")
public:
    explicit DesktopGUI();
    
    QRect getWindowRect();
    void setWindowRect(const QRect &rect);

    void setFullscreen(bool fullscreen);
    bool getFullscreen();

    void repaintGui();
    QList<QMenu*> getMenuItems();

    // Plugin interface

public slots:
    PLUGIN_ERROR_CODES initialize();
    PLUGIN_ERROR_CODES deinitialize();

protected:
    MainWindow* mainWindow;

    bool fullScreen;

    StbPlugin* configPlugin;
    Profile* configProfile;

    // GuiPlugin interface
public:


    // Plugin interface
public:
    void register_dependencies();
    void register_roles();
};

}

//Q_EXPORT_PLUGIN2(desktopgui, DesktopGUI)

#endif // DESKTOPGUI_H
