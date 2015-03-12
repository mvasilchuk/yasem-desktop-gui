#include "pluginsdialog.h"
#include "pluginmanager.h"
#include "plugin.h"

#include <QGridLayout>
#include <QDesktopWidget>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>

using namespace yasem;

PluginsDialog::PluginsDialog(QWidget *parent) :
    QDialog(parent)
{
    setupGui();
    updateTable();
}

void PluginsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            break;
        default:
            break;
    }
}

void PluginsDialog::setupGui()
{
    setWindowTitle(tr("Plugins"));
    setModal(true);
    setMinimumWidth(400);
    setMinimumHeight(300);

    move(QApplication::desktop()->screen()->rect().center() - rect().center());

    QVBoxLayout* main_layout = new QVBoxLayout(this);

    pluginsTable = new QTableWidget(this);
    main_layout->addWidget(pluginsTable);

    okButton = new QPushButton(tr("OK"), this);
    connect(okButton, &QPushButton::pressed, [=]() {
        close();
    });

    QHBoxLayout* hbox_layout = new QHBoxLayout();
    hbox_layout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    hbox_layout->addWidget(okButton);
    hbox_layout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    main_layout->addItem(hbox_layout);

    setLayout(main_layout);
}

void PluginsDialog::updateTable()
{
    pluginsTable->setRowCount(0);
    pluginsTable->setColumnCount(3);

    pluginsTable->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Plugin name")));
    pluginsTable->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Active")));
    pluginsTable->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Version")));

    QList<Plugin*> plugins = PluginManager::instance()->getPlugins(ROLE_ANY, false);
    int row_index = 0;
    for(Plugin* plugin: plugins)
    {
        pluginsTable->insertRow(pluginsTable->rowCount());
        QTableWidgetItem *nameItem = new QTableWidgetItem(plugin->getName());
        nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable);
        pluginsTable->setItem(row_index, 0, nameItem);

        QTableWidgetItem *activeItem = new QTableWidgetItem();
        activeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | /*Qt::ItemIsEnabled | */ Qt::ItemIsTristate);
        if(plugin->isActive())
        {
            if(plugin->getState() == PLUGIN_STATE_INITIALIZED)
                activeItem->setCheckState(Qt::Checked);
            else
                activeItem->setCheckState(Qt::PartiallyChecked);
        }
        else
            activeItem->setCheckState(Qt::Unchecked);
        activeItem->setTextAlignment(Qt::AlignCenter);
        pluginsTable->setItem(row_index, 1, activeItem);

        QTableWidgetItem *versionItem = new QTableWidgetItem(plugin->getVersion());
        versionItem->setFlags(versionItem->flags() ^ Qt::ItemIsEditable);
        versionItem->setTextAlignment(Qt::AlignCenter);
        pluginsTable->setItem(row_index, 2, versionItem);

        row_index++;
    }
    pluginsTable->resizeColumnsToContents();
    pluginsTable->resizeRowsToContents();
}

