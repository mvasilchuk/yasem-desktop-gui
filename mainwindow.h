#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
class QQuickWidget;

namespace yasem {

class DesktopGuiPlugin;
class BrowserPluginObject;
class DatasourcePlugin;
class GuiPluginObject;
class MediaPlayerPluginObject;
class NetworkStatistics;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();
    void resizeEvent(QResizeEvent *event);

    void initialize();

    virtual void browser(BrowserPluginObject* browserPlugin);
    virtual BrowserPluginObject* browser();

    virtual void datasource(DatasourcePlugin* datasourcePlugin);
    virtual DatasourcePlugin* datasource();

    virtual void gui(GuiPluginObject* guiPlugin);
    virtual GuiPluginObject* gui();

    virtual void player(MediaPlayerPluginObject* playerPlugin);
    virtual MediaPlayerPluginObject* player();

     QList<QMenu*> getMenuItems();
     QRect windowInternalRect();

protected:
    void changeEvent(QEvent *e);
    bool event(QEvent *event);

    DesktopGuiPlugin* desktopGui;
    QList<QMenu*> m_menuItems;

private:
    QMenuBar *menuBar;
    QToolBar* statusBarPanel;
    QStatusBar *statusBar;

    BrowserPluginObject* browserPlugin;
    DatasourcePlugin *datasourcePlugin;
    GuiPluginObject* guiPlugin;
    MediaPlayerPluginObject* playerPlugin;

    QScrollArea* errorScrollArea;
    QGroupBox* errorGroupBox;
    QVBoxLayout* errorBoxLayout;

    QLabel* currentProfileStatusBarLabel;
    QLabel* pageLoadingProgressStatusBarLabel;
    QQuickView* messageView;
    QPushButton* m_notification_icon;

    NetworkStatistics* m_network_statistics;
    QQuickWidget *m_statistics_view;

public slots:
    void setupPalette();
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
    void showStatistics(bool show);
    void updateStatistics();

signals:
    void fullScreenModeToggled(bool fullscreen);

};

}

#endif // MAINWINDOW_H
