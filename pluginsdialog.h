#ifndef PLUGINSDIALOG_H
#define PLUGINSDIALOG_H

#include <QTableWidget>
#include <QDialog>

namespace yasem {

class PluginsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PluginsDialog(QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);
    QTableWidget* pluginsTable;

    void setupGui();
    void updateTable();
};
}

#endif // PLUGINSDIALOG_H
