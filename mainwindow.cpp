#include "mainwindow.h"
#include "pluginmanager.h"
#include "desktopgui.h"
#include "core.h"
#include "profilemanager.h"
#include "browserplugin.h"
#include "stbprofile.h"


#include <QHBoxLayout>
#include <QStackedLayout>
#include <QHash>
#include <QMargins>
#include <QToolBar>

#include <QEvent>
#include <QKeyEvent>

using namespace yasem;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    QSettings* settings = Core::instance()->settings();
    settings->beginGroup("DesktopGui");
    restoreGeometry(settings->value("geometry").toByteArray());
    restoreState(settings->value("window_state").toByteArray());
    settings->endGroup();
}


void MainWindow::setupGui()
{
    Q_ASSERT(dynamic_cast<BrowserPlugin*>(browser()));
    Q_ASSERT(dynamic_cast<MediaPlayerPlugin*>(player()));

    QHBoxLayout* main = new QHBoxLayout;
    QStackedLayout* stackedLayout = new QStackedLayout;

    browser()->parent(this);
    stackedLayout->addWidget(browser()->widget());

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

    browser()->raise();

    //centralWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    setMouseTracking(true);
    browser()->setupMousePositionHandler(this, SLOT(onMousePositionChanged(int)));

    resizeWebView();
}

void MainWindow::setupMenu()
{
    //Setup menu
    menuBar = new QMenuBar(this);
    this->setMenuBar(menuBar);

    QMenu* fileMenu = new QMenu(tr("File"), this);

    QAction *exitAction = new QAction(tr("Exit"), this);
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);

    fileMenu->addAction(exitAction);

    QMenu* profilesMenu = new QMenu(tr("Profiles"), this);

    QAction *backToMainPage = new QAction(tr("Back to main page"), this);
    connect(backToMainPage, &QAction::triggered, []() {
        ProfileManager::instance()->backToMainPage();
    });

    profilesMenu->addAction(backToMainPage);

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(profilesMenu);



    //Setup status bar
    statusBar = new QStatusBar(this);
    //setStatusBar(statusBar);

    statusBarPanel = new QToolBar(this);
    statusBarPanel->setObjectName("statusBarPanel");
    statusBarPanel->addWidget(statusBar);
    statusBarPanel->setMovable(false);
    this->addToolBar(Qt::BottomToolBarArea, statusBarPanel);

    connect(ProfileManager::instance(), &ProfileManager::profileChanged, [=](Profile* profile) {
        statusBar->showMessage(tr("Profile:").append(profile->getName()));
    });


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
    browser()->fullscreen(fullscreen);
    //resizeWebView();
}

MainWindow::~MainWindow()
{
    //delete ui;

}


void MainWindow::initialize()
{
    DEBUG() << "initialize()";

    stb(dynamic_cast<StbPlugin*>(PluginManager::instance()->getByRole(ROLE_STB_API)));
    if(!stb())
    {
        ERROR() << "No STB plugin found! Portal won't work!";
    }
    datasource(dynamic_cast<DatasourcePlugin*>(PluginManager::instance()->getByRole(ROLE_DATASOURCE)));
    player(dynamic_cast<MediaPlayerPlugin*>(PluginManager::instance()->getByRole(ROLE_MEDIA)));
    gui(dynamic_cast<GuiPlugin*>(PluginManager::instance()->getByRole(ROLE_GUI)));
    Q_ASSERT(gui() != NULL);
    browser(dynamic_cast<BrowserPlugin*>(PluginManager::instance()->getByRole(ROLE_BROWSER)));

    setupGui();
    setupMenu();

    if(QCoreApplication::arguments().contains(arguments[FULLSCREEN_APP]))
    {
        setAppFullscreen(true);
    }

    QString id = "config";

    Profile* profile = ProfileManager::instance()->findById(id);
    Q_ASSERT(profile);
    ProfileManager::instance()->setActiveProfile(profile);

    //connect(Core::instance(), &Core::methodNotImplemented, this, &MainWindow::methodNotImplementedHandler);
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
        bool top = (position & MOUSE_POSITION::TOP) == MOUSE_POSITION::TOP;
        bool bottom = (position & MOUSE_POSITION::BOTTOM) == MOUSE_POSITION::BOTTOM;

        if(top && !menuBar->isVisible())
        {
           menuBar->setWindowFlags(Qt::WindowStaysOnTopHint);  // menu widget is child of mainwidget(central Widget of main window) and is not in layout
           menuBar->raise();
           menuBar->show();

           //resizeWebView();
        }
        else
        {
            if(!top && menuBar->isVisible())
            {
                menuBar->hide();
                browser()->parent()->resize(this->size());
                resizeWebView();
            }
        }

        if(bottom && !statusBar->isVisible())
        {
            this->removeToolBar(statusBarPanel);
            int stHeight = statusBarPanel->height();
            statusBarPanel->setGeometry(0, this->height() - stHeight, this->width(), stHeight);
            statusBarPanel->setStyleSheet("background:rgba(128, 128, 128, 0.2); color: black;");
            statusBarPanel->raise();
            statusBarPanel->show();
            //resizeWebView();
        }
        else if(!bottom && statusBar->isVisible())
        {
            statusBarPanel->setStyleSheet("");
            this->addToolBar(Qt::BottomToolBarArea, statusBarPanel);
            statusBarPanel->hide();
            browser()->parent()->resize(this->size());
            resizeWebView();
        }
    }
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
    player()->move(left, top);
}


void MainWindow::resizeWebView()
{
    //browser()->parent()->resize(this->size());
    browser()->resize();
    player()->setViewport(browser()->rect(), browser()->scale(), player()->fixedRect());
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

void MainWindow::addErrorToList(const QString &msg)
{
    QLabel* errorLabel = new QLabel(this);
    errorLabel->setText(msg);
    errorBoxLayout->addWidget(errorLabel);
}
