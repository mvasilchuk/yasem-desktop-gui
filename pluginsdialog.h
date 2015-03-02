#ifndef PLUGINSDIALOG_H
#define PLUGINSDIALOG_H

#include <QTableWidget>
#include <QDialog>
#include <QPushButton>

namespace yasem {

class PluginsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PluginsDialog(QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);
    QTableWidget* pluginsTable;
    QPushButton* okButton;

    void setupGui();
    void updateTable();
};
}

#endif // PLUGINSDIALOG_H
