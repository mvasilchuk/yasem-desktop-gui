#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "desktopgui.h"
#include "stbplugin.h"
#include "mediaplayerplugin.h"
#include "datasourceplugin.h"
#include "stbplugin.h"
#include "guiplugin.h"
#include "browserplugin.h"


#include <QMainWindow>

#include <QMenu>
#include <QStatusBar>
#include <QMenuBar>
#include <QLabel>
#include <QScrollArea>
#include <QGroupBox>
#include <QVBoxLayout>

namespace yasem {


class DesktopGUI;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();


    void resizeEvent(QResizeEvent *event);

    void initialize();

    virtual void browser(BrowserPlugin* browserPlugin);
    virtual BrowserPlugin* browser();

    virtual void datasource(DatasourcePlugin* datasourcePlugin);
    virtual DatasourcePlugin* datasource();

    virtual void stb(StbPlugin* stbPlugin);
    virtual StbPlugin* stb();

    virtual void gui(GuiPlugin* guiPlugin);
    virtual GuiPlugin* gui();

    virtual void player(MediaPlayerPlugin* playerPlugin);
    virtual MediaPlayerPlugin* player();

protected:
    void changeEvent(QEvent *e);
    bool event(QEvent *event);
    DesktopGUI* desktopGui;

private:



    QMenuBar *menuBar;
    QMenu *mainMenu;
    QMenu *profilesMenu;
    QStatusBar *statusBar;

    BrowserPlugin* browserPlugin;
    DatasourcePlugin *datasourcePlugin;
    StbPlugin *stbPlugin;
    GuiPlugin* guiPlugin;
    MediaPlayerPlugin* playerPlugin;

    QScrollArea* errorScrollArea;
    QGroupBox* errorGroupBox;
    QVBoxLayout* errorBoxLayout;


public slots:
    void closeEvent(QCloseEvent *event);

    void moveVideo(int left, int top);
    void resizeWebView();

    void setupGui();
    void setupMenu();
    void mouseDoubleClickEvent(QMouseEvent *e);

    void setAppFullscreen(bool fullscreen);

    void addErrorToList(const QString &msg);

};

}

#endif // MAINWINDOW_H
