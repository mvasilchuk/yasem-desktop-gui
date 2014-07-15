#include "desktopgui.h"
#include "pluginmanager.h"
#include "mainwindow.h"
#include "profilemanager.h"
#include "guistbobject.h"


#include <QApplication>

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

bool DesktopGUI::addWebObject(const QString &name, QWidget *widget, const QString &mimeType, const QString &classid, const QString &description)
{
    return true;
}

QUrl DesktopGUI::handleUrl(QUrl &url)
{
    return url;
}

void DesktopGUI::applyFixes()
{

}

QString DesktopGUI::getProfileClassId()
{
    return "config";
}

Profile *DesktopGUI::createProfile(const QString &id)
{
    return new GuiConfigProfile(this, id);
}

void DesktopGUI::init()
{
    getApi().clear();
    getApi().insert("__GUI__", new GuiStbObject(this));
}

PLUGIN_ERROR_CODES DesktopGUI::initialize()
{
    STUB();

    DEBUG() << "========== Starting desktop GUI ==========";

    gui(this);
    browser(dynamic_cast<BrowserPlugin*>(PluginManager::instance()->getByRole(ROLE_BROWSER)));

    ProfileManager::instance()->loadProfiles();
    configProfile = new GuiConfigProfile(dynamic_cast<StbPlugin*>(this), "config");
    configProfile->addFlag(Profile::HIDDEN); // Don't show in STB list
    Q_ASSERT(configProfile);
    ProfileManager::instance()->addProfile(configProfile);

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




QString yasem::DesktopGUI::getIcon(const QSize &size =  QSize())
{
    return "";
}


void yasem::DesktopGUI::register_dependencies()
{
    add_dependency(ROLE_BROWSER);
    add_dependency(ROLE_DATASOURCE);
    add_dependency(ROLE_MEDIA);
}

void yasem::DesktopGUI::register_roles()
{
    register_role(ROLE_GUI);
    register_role(ROLE_STB_API_SYSTEM);
}
