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

    QGridLayout* grid_layout = new QGridLayout();

    QString label_text = tr(
                   "<table>"
                   "<tr><th colspan=2 style='font-size: 18pt'>YASEM</th></tr>"
                   "<tr><td>Core version:</td>                      <td>%1 rev. %2</td></tr>"
                   "<tr><td>Qt version:</td>                        <td>%3</td></tr>"

                   "<tr></tr>"
                   "<tr><td colspan=2>Copyright &#169; 2014-%4 by Maxim Vasilchuk</td></tr>"
                   "<tr><td>Email:</td>                             <td><a href=\"mailto:mvasilchuk@gmail.com\">mvasilchuk@gmail.com</a></td></tr>"

                   "</table>");

    label_text = label_text
        .arg(Core::instance()->version())
        .arg(Core::instance()->revision().toUpper())
        .arg(qVersion())
        .arg(QDate::currentDate().year());


    QLabel* info_label = new QLabel(label_text, this);
    info_label->setTextFormat(Qt::RichText);

    grid_layout->addWidget(info_label);

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

