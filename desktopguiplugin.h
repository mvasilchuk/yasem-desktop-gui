#ifndef DESKTOPGUI_H
#define DESKTOPGUI_H

#include "desktopgui_global.h"
#include "guipluginobject.h"
#include "plugin.h"
#include <QMetaClassInfo>

class QMenu;

namespace yasem
{

class DESKTOPGUISHARED_EXPORT DesktopGuiPlugin: public Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.mvas.yasem.DesktopGuiPlugin/1.0" FILE "metadata.json")
    Q_INTERFACES(yasem::Plugin)

    Q_CLASSINFO("author", "Maxim Vasilchuk")
    Q_CLASSINFO("description", "Desktop GUI")
    Q_CLASSINFO("version", MODULE_VERSION)
    Q_CLASSINFO("revision", GIT_VERSION)
public:
    explicit DesktopGuiPlugin(QObject* parent = 0);
    virtual ~DesktopGuiPlugin();

    // Plugin interface
public:
    void register_dependencies();
    void register_roles();
};

}

#endif // DESKTOPGUI_H
