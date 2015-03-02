#include "aboutappdialog.h"

#include "core.h"

#include <QLabel>
#include <QGridLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QtWidgets/QVBoxLayout>
#include <QDate>
#include <QtWidgets/QSpacerItem>
#include <QPushButton>

using namespace yasem;

AboutAppDialog::AboutAppDialog(QWidget *parent) : QDialog(parent)
{
    setupGui();
}

AboutAppDialog::~AboutAppDialog()
{

}

void AboutAppDialog::setupGui()
{
    setWindowTitle(tr("About application..."));
    setModal(true);

    QVBoxLayout* vertical_layout = new QVBoxLayout(this);

    QGridLayout* grid_layout = new QGridLayout(this);
    QLabel* title = new QLabel(tr("Yasem"), this);
    title->setAlignment(Qt::AlignCenter);
    title->setFont(QFont("arial", 18, 4));

    QLabel* core_version = new QLabel(tr("Core version: %1").arg(Core::instance()->version()), this);
    QLabel* qt_version_compiled = new QLabel(tr("Compiled with Qt: %1").arg(QT_VERSION_STR));
    QLabel* qt_version_running = new QLabel(tr("Running with Qt: %1").arg(qVersion()));
    QLabel* copyright = new QLabel(tr("Copyright &#169; 2014-%1 by Maxim Vasilchuk").arg(QDate::currentDate().year()));
    copyright->setTextFormat(Qt::RichText);
    QLabel* email = new QLabel(tr("Email: <a href=\"mailto:mvasilchuk@gmail.com\">mvasilchuk@gmail.com</a>"));

    grid_layout->addWidget(title);
    grid_layout->addWidget(core_version);
    grid_layout->addWidget(qt_version_compiled);
    grid_layout->addWidget(qt_version_running);
    grid_layout->addWidget(copyright);
    grid_layout->addWidget(email);

    vertical_layout->addLayout(grid_layout);
    vertical_layout->addSpacerItem(new QSpacerItem(20, 80, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QHBoxLayout* hbox_layout = new QHBoxLayout(this);
    QPushButton* ok_button = new QPushButton(tr("OK"));
    connect(ok_button, &QPushButton::pressed, [=]() {
        close();
    });

    hbox_layout->addSpacerItem(new QSpacerItem(40, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
    hbox_layout->addWidget(ok_button);
    hbox_layout->addSpacerItem(new QSpacerItem(40, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
    vertical_layout->addLayout(hbox_layout);

    setLayout(vertical_layout);

    resize(vertical_layout->minimumSize());
    move(QApplication::desktop()->screen()->rect().center() - rect().center());
}

