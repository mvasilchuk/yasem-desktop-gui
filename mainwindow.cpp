#include "mainwindow.h"
#include "pluginmanager.h"
#include "desktopguiplugin.h"
#include "core.h"
#include "profilemanager.h"
#include "browser.h"
#include "stbprofile.h"
#include "pluginsdialog.h"
#include "aboutappdialog.h"
#include "mediaplayer.h"
#include "settingsdialog.h"
#include "openglwidgetcontainer.h"
#include "networkstatistics.h"
#include "statistics.h"
#include "yasemsettings.h"
#include "configuration_items.h"
#include "webpage.h"
#include "datasourcefactory.h"
#include "pluginmanager.h"

#include <QHBoxLayout>
#include <QStackedLayout>
#include <QHash>
#include <QMargins>
#include <QToolBar>
#include <QMenu>

#include <QEvent>
#include <QKeyEvent>
#include <QQmlEngine>
#include <QQuickView>
#include <QQmlComponent>
#include <QQmlContext>
#include <QPalette>
#include <QStyleFactory>
#include <QApplication>
#include <QOpenGLContext>
#include <QtQuickWidgets>

#if (QT_VERSION >= 0x050400)
#define USE_OPENGL_RENDER
#endif

using namespace yasem;

const QString SSL_STATUS_PROPERTY = "ssl_status";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_network_statistics(SDK::Core::instance()->statistics()->network()),
    m_network_statistics_enabled(true),
    m_opengl_enabled(SDK::Core::instance()->getVM() == SDK::Core::VM_NONE),
    m_ssl_status(SDK::Browser::SSL_UNDEFINED)
{
    m_statistics_view = NULL;

    QSettings* settings = SDK::Core::instance()->settings();
    settings->beginGroup("DesktopGui");
    restoreGeometry(settings->value("geometry").toByteArray());
    restoreState(settings->value("window_state").toByteArray());
    settings->endGroup();

    setupPalette();

    statusBarPanel = NULL;
    menuBar = NULL;

    //gui();
    //browser(NULL);
    //player(NULL);
    //datasource(NULL);
    messageView = NULL;
    m_notification_icon = NULL;
    m_network_statistics_enabled = SDK::Core::instance()->yasem_settings()->findItem(
                QStringList()
                    << SETTINGS_GROUP_OTHER
                    << NETWORK_STATISTICS
                    << NETWORK_STATISTICS_ENABLED)
            ->value().toBool();  

}

void MainWindow::setupGui()
{
    QWidget* centralWidget = NULL;
#ifdef USE_OPENGL_RENDER

    if(player() && player()->isSupportOpenGL() && m_opengl_enabled)
    {
        bool opengl_windget_created = false;
        do {
            // Enable OpenGL render
            centralWidget = new OpenGLWidgetContainer;
            QOpenGLWidget* opengl_widget = dynamic_cast<QOpenGLWidget*>(centralWidget);
            if(!opengl_widget)
            {
                WARN() << "OpenGL widget not created!";
                break;
            }

            QOpenGLContext* opengl_context = opengl_widget->context();
            if(!opengl_context)
            {
                WARN() << "Invalid OpenGL context!";
                break;
            }
            QPair<int,int> opengl_version = opengl_context->format().version();
            if(!opengl_widget->isValid() || opengl_version.first < 2) // Disable OpenGL for virtualbox
            {
                WARN() << "Invalid OpenGL version!";
                break;
            }
        } while(false);

        if(!opengl_windget_created)
        {
            WARN() << "Can't instantinate OpenGL widget. Falling back to QWidget";
            delete centralWidget;
            centralWidget = new QWidget(this);
        }
    }
    else
    {
        DEBUG() << "Player doesn't support OpenGL rendering. Creating a QWidget renderer";
        centralWidget = new QWidget(this);
    }

#else
    centralWidget = new QWidget(this);
#endif //USE_OPENGL_RENDER
    QStackedLayout* stackedLayout = new QStackedLayout;
    centralWidget->setLayout(stackedLayout);
    this->setCentralWidget(centralWidget);

    if(browser())
    {
        browser()->setParentWidget(centralWidget);
        SDK::WebPage* page = browser()->createNewPage();
        stackedLayout->addWidget(page->widget());
    }
    else
        WARN() << "Browser plugin not found! The app may not work correctly!";

    if(player() != NULL && player()->isInitialized())
    {
        player()->setAspectRatio(SDK::ASPECT_RATIO_AUTO);
        player()->parent(centralWidget);
        stackedLayout->addWidget(player()->widget());
        player()->show();
    }
    else
        WARN() << "No mediplayer plugin found. Media will be disabled!";

    if(browser())
        browser()->setTopWidget(SDK::Browser::TOP_WIDGET_BROWSER);

    browser()->widget()->raise();

    setMouseTracking(true);
    if(browser())
        browser()->setupMousePositionHandler(this, SLOT(onMousePositionChanged(int)));

    resizeWebView();
}

void MainWindow::setupMenu()
{
    //Setup menu
    menuBar = new QMenuBar(centralWidget());
    this->setMenuBar(menuBar);

    QMenu* fileMenu = new QMenu(tr("File"), menuBar);

    QAction *exitAction = new QAction(tr("Exit"), this);
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);

    fileMenu->addAction(exitAction);

    // Profiles

    QMenu* profilesMenu = new QMenu(tr("Profiles"));

    QAction *backToMainPage = new QAction(tr("Back to main page"), this);
    connect(backToMainPage, &QAction::triggered, []() {
        SDK::ProfileManager::instance()->backToMainPage();
    });

    profilesMenu->addAction(backToMainPage);
    profilesMenu->addSection(tr("Profiles"));

    connect(profilesMenu, &QMenu::triggered, [=](QAction* action) {
        const QSharedPointer<SDK::Profile>& profile = SDK::ProfileManager::instance()->findById(action->data().toString());
        if(profile.data() != NULL)
            SDK::ProfileManager::instance()->setActiveProfile(profile);
        else
            WARN() << qPrintable(QString("Can't load profile %1 from profiles menu!").arg(action->data().toString()));
    });

    connect(SDK::ProfileManager::instance(), &SDK::ProfileManager::profileChanged, [=](QSharedPointer<SDK::Profile> profile){
        for(QAction* action: profilesMenu->actions())
        {
            if(action->data().toString() == profile->getId())
            {
                action->setChecked(true);
                profilesMenu->setActiveAction(action);
            }
            else
            {
                action->setChecked(false);
            }
        }
    });

    for(const QSharedPointer<SDK::Profile>& profile: SDK::ProfileManager::instance()->getProfiles())
    {
        if(!profile->hasFlag(SDK::Profile::HIDDEN))
        {
            QAction* loadProfile = new QAction(profile->getName(), profilesMenu);
            loadProfile->setData(profile->getId());
            loadProfile->setCheckable(true);
            loadProfile->setChecked(false);
            profilesMenu->addAction(loadProfile);
        }
    }

    // Video menu
    QMenu* videoMenu = new QMenu(tr("Video"), menuBar);

    QMenu* aspectRatioMenu = new QMenu(tr("Aspect ratio"), videoMenu);

    connect(aspectRatioMenu, &QMenu::triggered, [=](QAction* action){
        if(player())
            player()->setAspectRatio((SDK::AspectRatio) action->data().toInt());
        aspectRatioMenu->setActiveAction(action);
        for(QAction* a: aspectRatioMenu->actions())
        {
            a->setChecked(a == action && player());
        }

    });

    connect(aspectRatioMenu, &QMenu::aboutToShow, [=]{
        if(!player())
            return;

        SDK::AspectRatio current_ratio = player()->getAspectRatio();
        for(QAction* action: aspectRatioMenu->actions())
        {
            SDK::AspectRatio ratio = (SDK::AspectRatio) action->data().toInt();
            if(ratio == current_ratio)
            {
                aspectRatioMenu->setActiveAction(action);
                action->setChecked(true);
                break;
            }
            else
            {
                action->setChecked(false);
            }
        }
    });

    QList<QPair<QString, SDK::AspectRatio>> ratios;
    ratios.append(QPair<QString, SDK::AspectRatio>(tr("Auto"),     SDK::ASPECT_RATIO_AUTO));
    ratios.append(QPair<QString, SDK::AspectRatio>(tr("1:1"),      SDK::ASPECT_RATIO_1_1));
    ratios.append(QPair<QString, SDK::AspectRatio>(tr("5:4"),      SDK::ASPECT_RATIO_5_4));
    ratios.append(QPair<QString, SDK::AspectRatio>(tr("4:3"),      SDK::ASPECT_RATIO_4_3));
    ratios.append(QPair<QString, SDK::AspectRatio>(tr("11:8"),     SDK::ASPECT_RATIO_11_8));
    ratios.append(QPair<QString, SDK::AspectRatio>(tr("14:10"),    SDK::ASPECT_RATIO_14_10));
    ratios.append(QPair<QString, SDK::AspectRatio>(tr("3:2"),      SDK::ASPECT_RATIO_3_2));
    ratios.append(QPair<QString, SDK::AspectRatio>(tr("14:9"),     SDK::ASPECT_RATIO_14_9));
    ratios.append(QPair<QString, SDK::AspectRatio>(tr("16:10"),    SDK::ASPECT_RATIO_16_10));
    ratios.append(QPair<QString, SDK::AspectRatio>(tr("16:9"),     SDK::ASPECT_RATIO_16_9));
    ratios.append(QPair<QString, SDK::AspectRatio>(tr("2.35:1"),   SDK::ASPECT_RATIO_2_35_1));

    for(QPair<QString, SDK::AspectRatio> pair: ratios)
    {
        QString name = pair.first;
        SDK::AspectRatio ratio = pair.second;
        QAction* action = new QAction(name, aspectRatioMenu);
        action->setData(ratio);
        action->setCheckable(true);
        action->setChecked(false);
        aspectRatioMenu->addAction(action);
    }

    videoMenu->addMenu(aspectRatioMenu);

    // Audio menu
    QMenu* audioMenu = new QMenu(tr("Audio"), menuBar);
    QMenu* audioTrackMenu = new QMenu(tr("Track"), audioMenu);

    connect(audioTrackMenu, &QMenu::triggered, [=](QAction* action){
        if(!player())
            return;

        player()->setAudioLanguage(action->data().toInt());
        audioTrackMenu->setActiveAction(action);
        for(QAction* a: audioTrackMenu->actions())
        {
            action->setChecked(a == action);
        }
    });

    connect(audioTrackMenu, &QMenu::aboutToShow, [=]{
        audioTrackMenu->clear();
        if(!player())
            return;

        int index = player()->getAudioPID();
        QList<AudioLangInfo> languages = player()->getAudioLanguages();
        for(const AudioLangInfo &lang: languages)
        {
            QAction* action = new QAction(lang.m_code3, audioTrackMenu);
            action->setData(lang.m_pid);
            action->setCheckable(true);
            action->setChecked(lang.m_pid == index);
            audioTrackMenu->addAction(action);
        }
    });

    audioMenu->addMenu(audioTrackMenu);

    // Settings

    QMenu* settingsMenu = new QMenu(tr("Settings"));
    QAction* settingsAction = new QAction(tr("Settings..."), settingsMenu);
    settingsAction->setMenuRole(QAction::PreferencesRole);
    connect(settingsAction, &QAction::triggered, [=]() {
        SettingsDialog* dialog = new SettingsDialog(this);
        dialog->setAttribute( Qt::WA_DeleteOnClose, true );
        dialog->show();
    });
    settingsMenu->addAction(settingsAction);

    QAction* toggleFullscreenAction = new QAction(tr("Fullscreen mode"), settingsMenu);
    toggleFullscreenAction->setCheckable(true);
    connect(this, &MainWindow::fullScreenModeToggled, [=](bool fullscreen){
        toggleFullscreenAction->setChecked(fullscreen);
    });
    connect(toggleFullscreenAction, &QAction::triggered, [=]() {
         setAppFullscreen(!isFullScreen());
    });
    settingsMenu->addAction(toggleFullscreenAction);


    //About

    QMenu* aboutMenu = new QMenu(tr("About"));

    QAction* aboutAppAction = new QAction(tr("About application..."), aboutMenu);
    aboutAppAction->setMenuRole(QAction::AboutRole);
    connect(aboutAppAction, &QAction::triggered, [=]() {
       AboutAppDialog dialog;
       dialog.exec();
       dialog.show();
    });


    QAction* aboutPluginsAction = new QAction(tr("About plugins..."), aboutMenu);
    aboutPluginsAction->setMenuRole(QAction::ApplicationSpecificRole);
    connect(aboutPluginsAction, &QAction::triggered, [=]() {
       PluginsDialog dialog;
       dialog.exec();
       dialog.show();
    });

    aboutMenu->addAction(aboutAppAction);
    aboutMenu->addAction(aboutPluginsAction);

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(videoMenu);
    menuBar->addMenu(audioMenu);
    menuBar->addMenu(profilesMenu);
    menuBar->addMenu(settingsMenu);
    menuBar->addMenu(aboutMenu);

    m_menuItems.append(fileMenu);
    m_menuItems.append(videoMenu);
    m_menuItems.append(audioMenu);
    m_menuItems.append(profilesMenu);
    m_menuItems.append(settingsMenu);
}



void MainWindow::setupStatusBar()
{
    //Setup status bar
    statusBar = new QStatusBar(this);
    //setStatusBar(statusBar);

    statusBarPanel = new QToolBar(this);
    statusBarPanel->setObjectName("statusBarPanel");
    statusBarPanel->addWidget(statusBar);
    statusBarPanel->setMovable(false);

    m_ssl_status_btn = new QPushButton(statusBar);
    m_ssl_status_btn->setFlat(true);

    connect(browser(), &SDK::Browser::page_loading_started, [=](const QString& url) {
        updateSslStatus(SDK::Browser::PLAINTEXT, url);
    });
    connect(browser(), &SDK::Browser::connection_encrypted, [=](const QString& url) {
        updateSslStatus(SDK::Browser::ENCRYPTED, url);
    });
    connect(browser(), &SDK::Browser::encryption_error, [=](const QString& url, const QList<QSslError> &errors) {
        updateSslStatus(SDK::Browser::SSL_ERROR, url, errors);
    });

    statusBar->addWidget(m_ssl_status_btn);

    currentProfileStatusBarLabel = new QLabel(statusBar);
    statusBar->addWidget(currentProfileStatusBarLabel);

    pageLoadingProgressStatusBarLabel = new QLabel(statusBar);
    statusBar->addWidget(pageLoadingProgressStatusBarLabel);

    this->addToolBar(Qt::BottomToolBarArea, statusBarPanel);

    connect(SDK::ProfileManager::instance(), &SDK::ProfileManager::profileChanged, [=](QSharedPointer<SDK::Profile> profile) {
        currentProfileStatusBarLabel->setText(tr("Profile:").append(profile->getName()));
    });

    // Disable network statistics for virtual machines, because OpenGL is not supported there correctly yet
    // (at least on VirtualBox)
    if(m_network_statistics_enabled && m_opengl_enabled)
    {
        m_notification_icon = new QPushButton(statusBar);
        m_notification_icon->setIcon(QIcon(":/res/icons/statistics_icon.png"));
        m_notification_icon->setFlat(true);
        m_notification_icon->setCheckable(true);
        m_notification_icon->setVisible(true);

        m_statistics_view = new QQuickWidget();

        QPair<int,int> opengl_version = m_statistics_view->format().version();
        DEBUG() << opengl_version;
        if(opengl_version.first < 2) // Disable OpenGL for virtualbox
        {
            WARN() << "Can't create QQuickWidget for statistics! Is it VirtualBox?";
            delete m_statistics_view;
        }
        else
        {
            m_statistics_view->setParent(this);
            m_statistics_view->setVisible(false);
        }

        connect(m_notification_icon, &QPushButton::toggled, [=](bool checked){
            showStatistics(checked);
        });
        statusBar->addWidget(m_notification_icon);

        connect(m_network_statistics, &SDK::NetworkStatistics::reseted, [=](){
            //showNotificationIcon(false);
        });
        connect(m_network_statistics, &SDK::NetworkStatistics::failedCountIncreased, [=](){
            showNotificationIcon(true);
        });
    }
    else
    {
        LOG() << "Statistics widget will be disabled, because it doesn't work on virtual machines.";
    }

    m_notification_label = new QLabel(statusBar);
    statusBar->addWidget(m_notification_label);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *e) {
    QWidget::mouseDoubleClickEvent(e);
    setAppFullscreen(!isFullScreen());
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    //qDebug() << "mousePressEvent" << e;
    //browser()->passEvent(e);
    // e->ignore();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    //qDebug() << "mouseReleaseEvent" << e;
    //browser()->passEvent(e);
    //e->ignore();
}


void MainWindow::setAppFullscreen(bool fullscreen)
{
    bool is_statistic_visible = m_statistics_view != NULL ? m_statistics_view->isVisible() : false;
    if(fullscreen) {
        if(is_statistic_visible)
        {
            showStatistics(false);
            m_notification_icon->setChecked(false);
        }
        this->setWindowState(Qt::WindowFullScreen);
        statusBarPanel->hide();
        menuBar->hide();

    } else {
        if(is_statistic_visible)
        {
            showStatistics(false);
            m_notification_icon->setChecked(false);
        }
        this->setWindowState(Qt::WindowNoState);
        statusBarPanel->show();
        menuBar->show();
    }
    if(browser())
        browser()->fullscreen(fullscreen);
    //resizeWebView();

    resizeWebView();

    emit fullScreenModeToggled(isFullScreen());
}

MainWindow::~MainWindow()
{

}


void MainWindow::initialize()
{
    DEBUG() << "MainWindow::initialize()";

    setupGui();
    setupMenu();
    setupStatusBar();

    connect(player(), &SDK::MediaPlayer::errorHappened, this, &MainWindow::onMediaPlayerError);

    if(QCoreApplication::arguments().contains(SDK::arguments[SDK::FULLSCREEN_APP]))
    {
        setAppFullscreen(true);
    }
}

void MainWindow::resizeEvent(QResizeEvent * )
{
    resizeWebView();
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        //ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool MainWindow::event(QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mEvent = (QMouseEvent*)event;
        if(mEvent->button() == Qt::RightButton)
        {
             event->ignore();
        }
    }
    else if(event->type() == QEvent::MouseMove)
    {
        event->ignore();
    }

    return QMainWindow::event(event);
}

void MainWindow::setupPalette()
{
    qDebug() << "Setting up dark palette...";
    qApp->setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25,25,25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    qApp->setPalette(darkPalette);

    qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

}

void MainWindow::onMousePositionChanged(int position)
{

    if(isFullScreen())
    {
        bool bottom = (position & SDK::MOUSE_POSITION::BOTTOM) == SDK::MOUSE_POSITION::BOTTOM;

        if(bottom && !statusBar->isVisible())
        {
            this->removeToolBar(statusBarPanel);
            int stHeight = statusBarPanel->height();
            statusBarPanel->setGeometry(0, this->height() - stHeight, this->width(), stHeight);
            statusBarPanel->setStyleSheet("background:rgba(128, 128, 128, 0.2); color: black;");
            statusBarPanel->raise();
            statusBarPanel->show();
        }
        else if(!bottom && statusBar->isVisible())
        {
            statusBarPanel->setStyleSheet("");
            this->addToolBar(Qt::BottomToolBarArea, statusBarPanel);
            statusBarPanel->hide();
            if(browser())
                browser()->getParentWidget()->resize(this->size());
            resizeWebView();
        }
    }
}

void MainWindow::checkDependencies()
{
    QStringList lines;

    for(QSharedPointer<SDK::Plugin> plugin: SDK::PluginManager::instance()->getPlugins(SDK::ROLE_ANY, false))
    {
        QStringList list;
        for(SDK::PluginDependency dependency: plugin->dependencies())
        {
            if(dependency.isRequired())
            {
                list.append(dependency.roleName());
            }
        }
        if(!list.isEmpty())
        {
            lines.append(QString("%1: %2").arg(plugin->getName()).arg(list.join(", ")));
        }
    }

    if(!lines.isEmpty())
    {
        if(messageView == NULL)
            messageView = new QQuickView(QUrl("qrc:/MessageWindow.qml"));
        QObject *rootObject = (QObject*)messageView->rootObject();
        QObject* plugins_table = rootObject->findChild<QObject*>("plugins_table");

        QMetaObject::invokeMethod(plugins_table, "readValues",
                Q_ARG(QVariant, QVariant::fromValue(lines)));
        messageView->show();
    }
}

void MainWindow::showNotificationIcon(bool show)
{
    m_notification_icon->setVisible(show);
}

void MainWindow::showStatistics(bool show)
{
    if(show)
    {
        connect(m_network_statistics, &SDK::NetworkStatistics::reseted, this, &MainWindow::updateStatistics);
        connect(m_network_statistics, &SDK::NetworkStatistics::totalCountIncreased, this, &MainWindow::updateStatistics);
        connect(m_network_statistics, &SDK::NetworkStatistics::successfulCountIncreased, this, &MainWindow::updateStatistics);
        connect(m_network_statistics, &SDK::NetworkStatistics::failedCountIncreased, this, &MainWindow::updateStatistics);
        connect(m_network_statistics, &SDK::NetworkStatistics::tooSlowCountIncreased, this, &MainWindow::updateStatistics);
        connect(m_network_statistics, &SDK::NetworkStatistics::pendingCountIncreased, this, &MainWindow::updateStatistics);
        connect(m_network_statistics, &SDK::NetworkStatistics::pendingCountDecreased, this, &MainWindow::updateStatistics);

        updateStatistics();
        if(m_statistics_view != NULL)
        {
            m_statistics_view->setSource(QUrl("qrc:/statistics_form.qml"));
            m_statistics_view->move(QPoint(m_notification_icon->geometry().left(), statusBarPanel->geometry().top() - m_statistics_view->height()));
            m_statistics_view->show();
        }

    }
    else
    {
        disconnect(m_network_statistics, &SDK::NetworkStatistics::reseted, this, &MainWindow::updateStatistics);
        disconnect(m_network_statistics, &SDK::NetworkStatistics::totalCountIncreased, this, &MainWindow::updateStatistics);
        disconnect(m_network_statistics, &SDK::NetworkStatistics::successfulCountIncreased, this, &MainWindow::updateStatistics);
        disconnect(m_network_statistics, &SDK::NetworkStatistics::failedCountIncreased, this, &MainWindow::updateStatistics);
        disconnect(m_network_statistics, &SDK::NetworkStatistics::tooSlowCountIncreased, this, &MainWindow::updateStatistics);
        disconnect(m_network_statistics, &SDK::NetworkStatistics::pendingCountIncreased, this, &MainWindow::updateStatistics);
        disconnect(m_network_statistics, &SDK::NetworkStatistics::pendingCountDecreased, this, &MainWindow::updateStatistics);
        if(m_statistics_view != NULL)
        {
            m_statistics_view->hide();
        }
    }
}

void MainWindow::updateStatistics()
{
    QQmlContext* context = m_statistics_view->rootContext();
    context->setContextProperty("network_total_count", m_network_statistics->totalCount());
    context->setContextProperty("network_successful_count", m_network_statistics->successfulCount());
    context->setContextProperty("network_failed_count", m_network_statistics->failedCount());
    context->setContextProperty("network_slow_count", m_network_statistics->tooSlowConnectionsCount());
    context->setContextProperty("network_pending_count", m_network_statistics->pendingConnectionsCount());
}

void MainWindow::loadStartPortal()
{
    QString configProfileSubmoduleId = "web-gui-config";

    const QSharedPointer<SDK::Profile>& profile = SDK::ProfileManager::instance()->findById(configProfileSubmoduleId);
    if(profile != NULL)
        SDK::ProfileManager::instance()->setActiveProfile(profile);
    else
        WARN() << qPrintable(QString("Can't load Web GUI configuration profile"));
}

void MainWindow::updateSslStatus(SDK::Browser::SslStatus status, const QString &url, const QList<QSslError> &errors)
{
    Q_UNUSED(url);
    if(status == m_ssl_status) return;

    switch(status)
    {
        case SDK::Browser::PLAINTEXT: {
            m_ssl_status_btn->setIcon(QIcon(":/res/icons/ssl/plaintext.png"));
            m_ssl_status_btn->setToolTip(tr("Connection is not encrypted"));
            break;
        }
        case SDK::Browser::ENCRYPTED: {
            if(m_ssl_status == SDK::Browser::SSL_ERROR) return;

            m_ssl_status_btn->setIcon(QIcon(":/res/icons/ssl/encrypted.png"));
            m_ssl_status_btn->setToolTip(tr("Connection to encrypted"));
            break;
        }
        case SDK::Browser::SSL_ERROR: {
            m_ssl_status_btn->setIcon(QIcon(":/res/icons/ssl/decrypted.png"));

            QStringList list;
            for(const QSslError& error: errors)
            {
               list.append(error.errorString());
            }
            m_ssl_status_btn->setToolTip(tr("Encryption error happened:<br>%1").arg(list.join("<br>")));
            break;
        }
        default: {
            break;
        }
    }
    m_ssl_status = status;
}

void MainWindow::onMediaPlayerError(SDK::MediaPlayer::MediaError error)
{
    switch(error)
    {
        case SDK::MediaPlayer::MEDIA_ERROR_NO_ERROR: {
            writeStatusMessage("OK");
            break;
        }
        case SDK::MediaPlayer::MEDIA_ERROR_RESOURCE_BUSY: {
            writeStatusMessage(tr("Media resource busy!"));
            break;
        }
        case SDK::MediaPlayer::MEDIA_ERROR_SERVICE_MISSING: {
            writeStatusMessage(tr("Media service missing!"));
            break;
        }
        case SDK::MediaPlayer::MEDIA_ERROR_RESOURCE_ERROR: {
            writeStatusMessage(tr("Media resource error!"));
            break;
        }
    }
}

void MainWindow::writeStatusMessage(const QString &msg)
{
    m_notification_label->setText(msg);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    DEBUG() << "CLOSE EVENT!!!!";

    QSettings* settings = SDK::Core::instance()->settings();
    settings->beginGroup("DesktopGui");
    settings->setValue("geometry", saveGeometry());
    settings->setValue("window_state", saveState());
    settings->endGroup();
    settings->sync();

    if(player() && player()->isInitialized())
        player()->mediaStop();

    QMainWindow::closeEvent(event);

    qApp->quit();
}

void MainWindow::moveVideo(int left, int top)
{
    if(player())
        player()->move(left, top);
}

void MainWindow::resizeWebView()
{
    if(browser()) browser()->resize();
    if(player() && player()->isInitialized())  player()->resize();
}

SDK::Browser* MainWindow::browser()
{
    return SDK::__get_plugin<SDK::Browser>(SDK::ROLE_BROWSER);
}

SDK::GUI* MainWindow::gui()
{
    return SDK::__get_plugin<SDK::GUI>(SDK::ROLE_GUI);
}

SDK::MediaPlayer* MainWindow::player()
{
    return SDK::__get_plugin<SDK::MediaPlayer>(SDK::ROLE_MEDIA);
}

QList<QMenu*> MainWindow::getMenuItems()
{
    return m_menuItems;
}

QRect MainWindow::windowInternalRect()
{
    if(isFullScreen())
        return rect();

    int height_diff = 0;
    if(statusBarPanel)
        height_diff = statusBarPanel->height();

    if(menuBar)
        height_diff += menuBar->height();

    QRect new_rect(rect().x(), rect().y(), rect().width(), rect().height() - height_diff);
    return new_rect;
}

void MainWindow::addErrorToList(const QString &msg)
{
    QLabel* errorLabel = new QLabel(this);
    errorLabel->setText(msg);
    errorBoxLayout->addWidget(errorLabel);
}
