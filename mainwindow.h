#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "browser.h"
#include "mediaplayer.h"

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

namespace SDK {
class DesktopGuiPlugin;
class Browser;
class Datasource;
class GUI;
class MediaPlayer;
class NetworkStatistics;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();
    void resizeEvent(QResizeEvent *event);

    void initialize();
    void setupWindowSize();

    virtual SDK::Browser* browser();
    virtual SDK::GUI* gui();
    virtual SDK::MediaPlayer* player();

     QList<QMenu*> getMenuItems();
     QRect windowInternalRect();

protected:
    void changeEvent(QEvent *e);
    bool event(QEvent *event);

    SDK::DesktopGuiPlugin* desktopGui;
    QList<QMenu*> m_menuItems;

protected slots:
    void mouseMoveEvent(QMouseEvent *e);

private:
    QMenuBar *menuBar;
    QToolBar* statusBarPanel;
    QStatusBar *statusBar;

    QSharedPointer<SDK::Browser> m_browser_plugin;
    QSharedPointer<SDK::Datasource> m_datasource_plugin;
    QSharedPointer<SDK::GUI> m_gui_plugin;
    QSharedPointer<SDK::MediaPlayer> m_player_plugin;

    QScrollArea* errorScrollArea;
    QGroupBox* errorGroupBox;
    QVBoxLayout* errorBoxLayout;

    QLabel* currentProfileStatusBarLabel;
    QLabel* pageLoadingProgressStatusBarLabel;
    QQuickView* messageView;
    QPushButton* m_ssl_status_btn;
    QPushButton* m_notification_icon;
    QLabel* m_notification_label;
    QSettings* m_settings;

    SDK::NetworkStatistics* m_network_statistics;
    bool m_network_statistics_enabled;
    bool m_opengl_enabled;

    QQuickWidget *m_statistics_view;
    SDK::Browser::SslStatus m_ssl_status;
    int m_mouse_border_threshold;

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
    void checkDependencies();
    void showNotificationIcon(bool show);
    void showStatistics(bool show);
    void updateStatistics();
    void loadStartPortal();
    void updateSslStatus(SDK::Browser::SslStatus status, const QString& url, const QList<QSslError> &errors = QList<QSslError>());
    void onMediaPlayerError(SDK::MediaPlayer::MediaError error);
    void writeStatusMessage(const QString& msg);

signals:
    void fullScreenModeToggled(bool fullscreen);

};

}

#endif // MAINWINDOW_H
