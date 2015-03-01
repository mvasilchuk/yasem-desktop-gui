#include "pluginsdialog.h"
#include "pluginmanager.h"

#include <QGridLayout>
#include <QDesktopWidget>
#include <QApplication>

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
    setMinimumWidth(600);
    setMinimumHeight(400);

    move(QApplication::desktop()->screen()->rect().center() - rect().center());

    pluginsTable = new QTableWidget(this);

    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->addWidget(pluginsTable);

    setLayout(gridLayout);

    pluginsTable->setRowCount(0);
    pluginsTable->setColumnCount(3);

    pluginsTable->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Plugin name")));
    pluginsTable->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Active")));
    pluginsTable->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Version")));

    pluginsTable->setColumnWidth(0, 300);
}

void PluginsDialog::updateTable()
{
    pluginsTable->setRowCount(0);
    QList<Plugin*> plugins = PluginManager::instance()->getPlugins(ROLE_ANY, false);
    int row_index = 0;
    for(Plugin* plugin: plugins)
    {
        pluginsTable->insertRow(pluginsTable->rowCount());
        QTableWidgetItem *nameItem = new QTableWidgetItem(plugin->getName());
        nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable);
        pluginsTable->setItem(row_index, 0, nameItem);

        QTableWidgetItem *activeItem = new QTableWidgetItem();
        activeItem->setFlags(Qt::ItemIsUserCheckable);
        if(plugin->isActive())
            activeItem->setCheckState(Qt::Checked);
        else
            activeItem->setCheckState(Qt::Unchecked);
        activeItem->setTextAlignment(Qt::AlignCenter);
        pluginsTable->setItem(row_index, 1, activeItem);

        QTableWidgetItem *versionItem = new QTableWidgetItem(plugin->getVersion());
        versionItem->setFlags(versionItem->flags() ^ Qt::ItemIsEditable);
        pluginsTable->setItem(row_index, 2, versionItem);

        row_index++;
    }
}

