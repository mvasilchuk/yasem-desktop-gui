#include "mainwindow.h"
#include "pluginmanager.h"
#include "desktopgui.h"
#include "core.h"
#include "profilemanager.h"


#include <QHBoxLayout>
#include <QStackedLayout>
#include <QHash>
#include <QMargins>

#include <QEvent>
#include <QKeyEvent>

using namespace yasem;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

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
        ERROR("No mediplayer plugin found. Media will be disabled!");

    main->addLayout(stackedLayout);
    QWidget* centralWidget = new QWidget();
    centralWidget->setLayout(main);

    this->setCentralWidget(centralWidget);
    if(player())
        player()->raise();


    browser()->raise();
    resizeWebView();
}

void MainWindow::setupMenu()
{
    menuBar = new QMenuBar(this);
    this->setMenuBar(menuBar);

    mainMenu = new QMenu(tr("File"), this);

    QAction *exitAction = new QAction(tr("Exit"), this);
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);

    mainMenu->addAction(exitAction);

    profilesMenu = new QMenu(tr("Profiles"), this);

    menuBar->addMenu(mainMenu);


    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *e) {
    QWidget::mouseDoubleClickEvent(e);
    setAppFullscreen(!isFullScreen());
}

void MainWindow::setAppFullscreen(bool fullscreen)
{
    if(fullscreen) {
        this->setWindowState(Qt::WindowFullScreen);
        statusBar->hide();
        menuBar->hide();
    } else {
        this->setWindowState(Qt::WindowNoState);
        statusBar->show();
        menuBar->show();
    }
    resizeWebView();
}

MainWindow::~MainWindow()
{
    //delete ui;
}


void MainWindow::initialize()
{
    DEBUG("initialize()");

    stb(dynamic_cast<StbPlugin*>(PluginManager::instance()->getByRole("stbapi")));
    if(!stb())
    {
        ERROR("No STB plugin found! Portal won't work!");
    }
    datasource(dynamic_cast<DatasourcePlugin*>(PluginManager::instance()->getByRole("datasource")));
    player(dynamic_cast<MediaPlayerPlugin*>(PluginManager::instance()->getByRole("mediaplayer")));
    gui(dynamic_cast<GuiPlugin*>(PluginManager::instance()->getByRole("gui")));
    Q_ASSERT(gui() != NULL);
    browser(dynamic_cast<BrowserPlugin*>(PluginManager::instance()->getByRole("browser")));

    setupGui();
    setupMenu();

    if(QCoreApplication::arguments().contains(arguments[FULLSCREEN_APP]))
        setAppFullscreen(true);

    QString id = "config";

    Profile* profile = ProfileManager::instance()->findById(id);
    Q_ASSERT(profile);
    ProfileManager::instance()->setActiveProfile(profile);

    //connect(Core::instance(), &Core::methodNotImplemented, this, &MainWindow::methodNotImplementedHandler);
}

void MainWindow::methodNotImplementedHandler(const QString& name)
{
    DEBUG("NOT IMPLEMENTED: " + name);
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

    return QMainWindow::event(event);
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    /*QSettings settings("MyCompany", "MyApp");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());*/
    if(player())
        player()->mediaStop();
    QMainWindow::closeEvent(event);
}

void MainWindow::moveVideo(int left, int top)
{
    player()->move(left, top);
}


void MainWindow::resizeWebView()
{
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
