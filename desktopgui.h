#ifndef DESKTOPGUI_H
#define DESKTOPGUI_H

#include "desktopgui_global.h"
#include "guiplugin.h"
#include "mediaplayerplugin.h"
#include "datasourceplugin.h"
#include "stbplugin.h"
#include "guiconfigprofile.h"

namespace yasem
{

class MainWindow;

class DESKTOPGUISHARED_EXPORT DesktopGUI: public QObject, public virtual Plugin, public GuiPlugin, public StbPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.mvas.yasem.DesktopGuiPlugin/1.0" FILE "metadata.json")
    Q_INTERFACES(yasem::Plugin yasem::GuiPlugin yasem::StbPlugin)

    Q_CLASSINFO("author", "Maxim Vasilchuk")
    Q_CLASSINFO("description", "Desktop GUI for YASEM")
public:
    explicit DesktopGUI();

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
    QRect getWindowRect();
    void setWindowRect(const QRect &rect);

    void setFullscreen(bool fullscreen);
    bool getFullscreen();

    // StbPlugin interface
public slots:
    bool addWebObject(const QString &name, QWidget *widget, const QString &mimeType, const QString &classid, const QString &description);
    QUrl handleUrl(QUrl &url);
    void applyFixes();

    QString getProfileClassId();
    Profile *createProfile(const QString &id);
    void init();
};

}

//Q_EXPORT_PLUGIN2(desktopgui, DesktopGUI)

#endif // DESKTOPGUI_H
