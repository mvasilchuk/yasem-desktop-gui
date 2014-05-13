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
        //player()->mediaPlay("file:///win/d/torrents/Pipez_(2010).HDRip-AVC/Pipez_(2010).HDRip-AVC.aac_[All.Films][RG].mkv");

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

    /*QLabel* lbl = new QLabel( player()->widget());
    lbl->setText("HELLO!!!");
    stackedLayout->addWidget(lbl);
    lbl->setGeometry(100, 100, 100, 100);
    lbl->show();
    lbl->raise();
    */
    /*QPalette palette = this->palette();
    palette.setBrush(QPalette::Base, Qt::transparent);
    lbl->setPalette(palette);
    lbl->setAttribute(Qt::WA_OpaquePaintEvent, false);
    lbl->setAttribute(Qt::WA_TranslucentBackground, true);
    //setAttribute(Qt::WA_NoSystemBackground, true);
    lbl->setStyleSheet("background: none;");*/

    //browser()->raise();


    /*
    errorScrollArea = new QScrollArea(this);
    errorScrollArea->setGeometry(0, 0, 100, 100);

    errorGroupBox = new QGroupBox();
    errorBoxLayout = new QVBoxLayout();
    errorBoxLayout->setAlignment(Qt::AlignTop);


    errorBoxLayout->setSizeConstraint(QLayout::SetMaximumSize);
    errorGroupBox->setLayout(errorBoxLayout);
    errorScrollArea->setWidget(errorGroupBox);

    errorScrollArea->setWidgetResizable(true);
    errorGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    errorScrollArea->setWidgetResizable(true);
    errorScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    errorScrollArea->setMaximumHeight(40);
    errorScrollArea->setMaximumHeight(300);

    addErrorToList("test");
    addErrorToList("test2");
    addErrorToList("test3");
    addErrorToList("test4");
    addErrorToList("test5");*/
}

void MainWindow::setupMenu()
{
    menuBar = new QMenuBar(this);
    this->setMenuBar(menuBar);

    mainMenu = new QMenu(this);
    mainMenu->setTitle(tr("File"));

    QAction *exitAction = new QAction(tr("Exit"), this);
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);

    mainMenu->addAction(exitAction);
    menuBar->addMenu(mainMenu);

    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);

    profilesMenu = new QMenu(this);
    profilesMenu->setTitle(tr("Profiles"));
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *e) {
    QWidget::mouseDoubleClickEvent(e);
    setAppFullscreen(!isFullScreen());
}

void MainWindow::setAppFullscreen(bool fullscreen)
{
    if(fullscreen) {
        this->setWindowState(Qt::WindowFullScreen);
        browser()->resize();
        statusBar->hide();
        menuBar->hide();
    } else {
        this->setWindowState(Qt::WindowNoState);
        browser()->resize();
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

    //QString id = "88b6d0a7-6211-4c09-aafd-46748de89e";
    QString id = "config";

    Profile* profile = ProfileManager::instance()->findById(id);
    Q_ASSERT(profile);
    ProfileManager::instance()->setActiveProfile(profile);
}

void MainWindow::resizeEvent(QResizeEvent * /* event */)
{
    resizeWebView();
    //player()->widget()->resize(browser()->rect().width(), browser()->rect().height());
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
                return true;
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
