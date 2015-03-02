#include "mainwindow.h"
#include "pluginmanager.h"
#include "desktopgui.h"
#include "core.h"
#include "profilemanager.h"
#include "browserplugin.h"
#include "stbprofile.h"
#include "pluginsdialog.h"
#include "aboutappdialog.h"


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

using namespace yasem;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    QSettings* settings = Core::instance()->settings();
    settings->beginGroup("DesktopGui");
    restoreGeometry(settings->value("geometry").toByteArray());
    restoreState(settings->value("window_state").toByteArray());
    settings->endGroup();

    statusBarPanel = NULL;
    menuBar = NULL;

    gui(NULL);
    browser(NULL);
    player(NULL);
    datasource(NULL);
    messageView = NULL;
    notificationIconBtn = NULL;
}

void MainWindow::setupGui()
{
    QHBoxLayout* main = new QHBoxLayout;
    QStackedLayout* stackedLayout = new QStackedLayout;

    if(browser())
    {
        browser()->setParentWidget(this);
        stackedLayout->addWidget(browser()->widget());
    }
    else
        ERROR() << "Browser plugin not found! The app may not work correctly!";

    if(player() != NULL)
    {
        player()->aspectRatio(ASPECT_RATIO_AUTO);
        player()->parent(this);
        stackedLayout->addWidget(player()->widget());
        player()->show();

    }
    else
        ERROR() << "No mediplayer plugin found. Media will be disabled!";

    main->addLayout(stackedLayout);
    QWidget* centralWidget = new QWidget();
    centralWidget->setLayout(main);

    this->setCentralWidget(centralWidget);
    if(player())
        player()->raise();

    if(browser())
        browser()->raise();

    //centralWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
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
        ProfileManager::instance()->backToMainPage();
    });

    profilesMenu->addAction(backToMainPage);
    profilesMenu->addSection(tr("Profiles"));

    connect(profilesMenu, &QMenu::triggered, [=](QAction* action) {
        Profile* profile = ProfileManager::instance()->findById(action->data().toString());
        if(profile != NULL)
            ProfileManager::instance()->setActiveProfile(profile);
        else
            WARN() << qPrintable(QString("Can't load profile %1 from profiles menu!").arg(action->data().toString()));
    });

    connect(ProfileManager::instance(), &ProfileManager::profileChanged, [=](Profile* profile){
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

    for(Profile* profile: ProfileManager::instance()->getProfiles())
    {
        if(!profile->hasFlag(Profile::HIDDEN))
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
            player()->aspectRatio((ASPECT_RATIO) action->data().toInt());
        aspectRatioMenu->setActiveAction(action);
        for(QAction* a: aspectRatioMenu->actions())
        {
            a->setChecked(a == action && player());
        }

    });

    connect(aspectRatioMenu, &QMenu::aboutToShow, [=]{
        if(!player())
            return;

        ASPECT_RATIO current_ratio = player()->aspectRatio();
        for(QAction* action: aspectRatioMenu->actions())
        {
            ASPECT_RATIO ratio = (ASPECT_RATIO) action->data().toInt();
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

    QList<QPair<QString, ASPECT_RATIO>> ratios;
    ratios.append(QPair<QString, ASPECT_RATIO>(tr("Auto"),     ASPECT_RATIO_AUTO));
    ratios.append(QPair<QString, ASPECT_RATIO>(tr("1:1"),      ASPECT_RATIO_1_1));
    ratios.append(QPair<QString, ASPECT_RATIO>(tr("5:4"),      ASPECT_RATIO_5_4));
    ratios.append(QPair<QString, ASPECT_RATIO>(tr("4:3"),      ASPECT_RATIO_4_3));
    ratios.append(QPair<QString, ASPECT_RATIO>(tr("11:8"),     ASPECT_RATIO_11_8));
    ratios.append(QPair<QString, ASPECT_RATIO>(tr("14:10"),    ASPECT_RATIO_14_10));
    ratios.append(QPair<QString, ASPECT_RATIO>(tr("3:2"),      ASPECT_RATIO_3_2));
    ratios.append(QPair<QString, ASPECT_RATIO>(tr("14:9"),     ASPECT_RATIO_14_9));
    ratios.append(QPair<QString, ASPECT_RATIO>(tr("16:10"),    ASPECT_RATIO_16_10));
    ratios.append(QPair<QString, ASPECT_RATIO>(tr("16:9"),     ASPECT_RATIO_16_9));
    ratios.append(QPair<QString, ASPECT_RATIO>(tr("2.35:1"),  ASPECT_RATIO_2_35_1));

    for(QPair<QString, ASPECT_RATIO> pair: ratios)
    {
        QString name = pair.first;
        ASPECT_RATIO ratio = pair.second;
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

        int index = player()->audioPID();
        QList<AudioLangInfo> languages = player()->getAudioLanguages();
        for(const AudioLangInfo &lang: languages)
        {
            QAction* action = new QAction(lang.code3, audioTrackMenu);
            action->setData(lang.pid);
            action->setCheckable(true);
            action->setChecked(lang.pid == index);
            audioTrackMenu->addAction(action);
        }
    });

    audioMenu->addMenu(audioTrackMenu);

    //About

    QMenu* aboutMenu = new QMenu(tr("About"));

    QAction* aboutAppAction = new QAction(tr("About application..."), aboutMenu);
    connect(aboutAppAction, &QAction::triggered, [=]() {
       AboutAppDialog dialog;
       dialog.exec();
       dialog.show();
    });


    QAction* aboutPluginsAction = new QAction(tr("About plugins..."), aboutMenu);
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
    menuBar->addMenu(aboutMenu);

    m_menuItems.append(fileMenu);
    m_menuItems.append(videoMenu);
    m_menuItems.append(audioMenu);
    m_menuItems.append(profilesMenu);  
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

    currentProfileStatusBarLabel = new QLabel(statusBar);
    statusBar->addWidget(currentProfileStatusBarLabel);

    pageLoadingProgressStatusBarLabel = new QLabel(statusBar);
    statusBar->addWidget(pageLoadingProgressStatusBarLabel);

    this->addToolBar(Qt::BottomToolBarArea, statusBarPanel);

    connect(ProfileManager::instance(), &ProfileManager::profileChanged, [=](Profile* profile) {
        currentProfileStatusBarLabel->setText(tr("Profile:").append(profile->getName()));
    });

    notificationIconBtn = new QPushButton(statusBar);
    notificationIconBtn->setIcon(this->style()->standardIcon(QStyle::SP_MessageBoxWarning));
    notificationIconBtn->setFlat(true);
    notificationIconBtn->setVisible(false);
    statusBar->addWidget(notificationIconBtn);
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
    if(fullscreen) {
        this->setWindowState(Qt::WindowFullScreen);
        statusBarPanel->hide();
        menuBar->hide();

    } else {
        this->setWindowState(Qt::WindowNoState);
        statusBarPanel->show();
        menuBar->show();
    }
    if(browser())
        browser()->fullscreen(fullscreen);
    //resizeWebView();
}

MainWindow::~MainWindow()
{

}


void MainWindow::initialize()
{
    DEBUG() << "MainWindow::initialize()";

    stb(dynamic_cast<StbPlugin*>(PluginManager::instance()->getByRole(ROLE_STB_API)));
    if(!stb())
    {
        ERROR() << "No STB plugin found! Portal won't work!";
    }
    datasource(dynamic_cast<DatasourcePlugin*>(PluginManager::instance()->getByRole(ROLE_DATASOURCE)));
    player(dynamic_cast<MediaPlayerPlugin*>(PluginManager::instance()->getByRole(ROLE_MEDIA)));
    gui(dynamic_cast<GuiPlugin*>(PluginManager::instance()->getByRole(ROLE_GUI)));
    browser(dynamic_cast<BrowserPlugin*>(PluginManager::instance()->getByRole(ROLE_BROWSER)));

    if(browser())
        browser()->createNewPage();

    setupGui();
    setupMenu();
    setupStatusBar();

    if(QCoreApplication::arguments().contains(arguments[FULLSCREEN_APP]))
    {
        setAppFullscreen(true);
    }

    QString configProfileSubmoduleId = "web-gui-config";
    StbPlugin* stbPlugin = dynamic_cast<StbPlugin*>(PluginManager::instance()->getByRole(ROLE_WEB_GUI));
    Q_ASSERT_X(stbPlugin, "MainWindow::initialize", "Web GUI STB plugin not found!");

    Profile* profile = ProfileManager::instance()->findById(configProfileSubmoduleId);
    if(profile != NULL)
        ProfileManager::instance()->setActiveProfile(profile);
    else
        WARN() << qPrintable(QString("Can't load Web GUI configuration profile"));

    //checkDependencies();
}

void MainWindow::resizeEvent(QResizeEvent * /* event */)
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
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
        switch(keyEvent->key())
        {
            case Qt::Key_F11:
            {
                gui()->setFullscreen(!gui()->getFullscreen());
                return false;
            }
        }
    }
    else if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mEvent = (QMouseEvent*)event;
        if(mEvent->button() == Qt::RightButton)
        {
             event->ignore();
        }
        //return false;
    }
    else if(event->type() == QEvent::MouseMove)
    {
        event->ignore();
    }

    return QMainWindow::event(event);
}

void MainWindow::onMousePositionChanged(int position)
{

    if(isFullScreen())
    {
        bool bottom = (position & MOUSE_POSITION::BOTTOM) == MOUSE_POSITION::BOTTOM;

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

    for(Plugin* plugin: PluginManager::instance()->getPlugins(ROLE_ANY, false))
    {
        QStringList list;
        for(PluginDependency dependency: plugin->dependencies())
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
    notificationIconBtn->setVisible(show);
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    DEBUG() << "CLOSE EVENT!!!!";

    QSettings* settings = Core::instance()->settings();
    settings->beginGroup("DesktopGui");
    settings->setValue("geometry", saveGeometry());
    settings->setValue("window_state", saveState());
    settings->endGroup();
    settings->sync();

    if(player())
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
    if(browser())
    {
        browser()->resize();
        if(player())
            player()->setViewport(browser()->rect(), browser()->scale(), player()->fixedRect());
    }
}

void MainWindow::browser(BrowserPlugin *browserPlugin)
{
    this->browserPlugin = browserPlugin;
}

BrowserPlugin *MainWindow::browser()
{
    return this->browserPlugin;
}

void MainWindow::datasource(DatasourcePlugin *datasourcePlugin)
{
    this->datasourcePlugin = datasourcePlugin;
}

DatasourcePlugin *MainWindow::datasource()
{
    return this->datasourcePlugin;
}

void MainWindow::stb(StbPlugin *stbPlugin)
{
    this->stbPlugin = stbPlugin;
}

StbPlugin *MainWindow::stb()
{
    return this->stbPlugin;
}

void MainWindow::gui(GuiPlugin *guiPlugin)
{
    this->guiPlugin = guiPlugin;
}

GuiPlugin *MainWindow::gui()
{
    return  this->guiPlugin;
}

void MainWindow::player(MediaPlayerPlugin *playerPlugin)
{
    this->playerPlugin = playerPlugin;
}

MediaPlayerPlugin *MainWindow::player()
{
    return this->playerPlugin;
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
