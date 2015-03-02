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
#include <QPushButton>

class QQuickView;

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

     QList<QMenu*> getMenuItems();
     QRect windowInternalRect();

protected:
    void changeEvent(QEvent *e);
    bool event(QEvent *event);

    DesktopGUI* desktopGui;
    QList<QMenu*> m_menuItems;

private:
    QMenuBar *menuBar;
    QToolBar* statusBarPanel;
    QStatusBar *statusBar;

    BrowserPlugin* browserPlugin;
    DatasourcePlugin *datasourcePlugin;
    StbPlugin *stbPlugin;
    GuiPlugin* guiPlugin;
    MediaPlayerPlugin* playerPlugin;

    QScrollArea* errorScrollArea;
    QGroupBox* errorGroupBox;
    QVBoxLayout* errorBoxLayout;

    QLabel* currentProfileStatusBarLabel;
    QLabel* pageLoadingProgressStatusBarLabel;
    QQuickView* messageView;
    QPushButton* notificationIconBtn;


public slots:
    void closeEvent(QCloseEvent *event);

    void moveVideo(int left, int top);
    void resizeWebView();

    void setupGui();
    void setupMenu();
    void setupStatusBar();
    void mouseDoubleClickEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent *e);

    void setAppFullscreen(bool fullscreen);

    void addErrorToList(const QString &msg);
    void onMousePositionChanged(int position);

    void checkDependencies();
    void showNotificationIcon(bool show);

};

}

#endif // MAINWINDOW_H
