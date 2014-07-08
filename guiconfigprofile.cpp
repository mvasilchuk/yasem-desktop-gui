#include "guiconfigprofile.h"
#include "stbplugin.h"
#include "browserplugin.h"

using namespace yasem;

GuiConfigProfile::GuiConfigProfile(StbPlugin* profilePlugin, const QString &id) :
    Profile(profilePlugin, id)
{
}

void GuiConfigProfile::start()
{
    STUB();
    configureKeyMap();
    profilePlugin->browser()->stb(profilePlugin);
    profilePlugin->browser()->setInnerSize(QSize(1920, 1080));
    profilePlugin->browser()->load(QUrl(portal()));
}

void GuiConfigProfile::stop()
{

}

void GuiConfigProfile::initDefaults()
{

}

void GuiConfigProfile::configureKeyMap()
{
    profilePlugin->browser()->clearKeyEvents();
    //profilePlugin->browser()->registerKeyEvent(RC_KEY_LEFT, 37, 37);
    //profilePlugin->browser()->registerKeyEvent(RC_KEY_UP, 38, 38);
    //profilePlugin->browser()->registerKeyEvent(RC_KEY_RIGHT, 39, 39);
    //profilePlugin->browser()->registerKeyEvent(RC_KEY_DOWN, 40, 40);
    profilePlugin->browser()->registerKeyEvent(RC_KEY_OK, 13, 13);

    profilePlugin->browser()->registerKeyEvent(RC_KEY_RED, 11, 11);

    profilePlugin->browser()->registerKeyEvent(RC_KEY_EXIT, 27, 27); //ESC

    profilePlugin->browser()->registerKeyEvent(RC_KEY_RED, 112, 112); //F1 / Red
    profilePlugin->browser()->registerKeyEvent(RC_KEY_GREEN, 113, 113); //F2 / Green
    profilePlugin->browser()->registerKeyEvent(RC_KEY_YELLOW, 114, 114); //F3 / Yellow
    profilePlugin->browser()->registerKeyEvent(RC_KEY_BLUE, 115, 115); //F4 / Blue
}

QString GuiConfigProfile::portal()
{
    return "qrc:/desktop/gui/res/html/main.html";
}
