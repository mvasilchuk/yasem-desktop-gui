#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLayout>
#include <QTreeView>
#include <QStandardItemModel>
#include <QPushButton>

class QStandardItem;

namespace yasem
{

namespace SDK {
class YasemSettings;
class ConfigTreeGroup;
class ConfigContainer;
class ConfigItem;
}

struct ConfigContainerHelper {
    SDK::ConfigContainer* container;
};

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);
    void setupDialog();
    void updateConfigGroups();
    QStandardItem* addTreeItem(SDK::ConfigTreeGroup* group);

public slots:
    void updateConfigPage(const QModelIndex &index);
    void cancel();

protected slots:
     SDK::ConfigItem* getItemFromObject(QObject* object);

     void onConfigDataTextChanged(const QString &text);
     void onConfigDataBoolChanged(bool checked);
     void onConfigDataListItemChanged(int index);

protected:


    SDK::YasemSettings* m_settings;
    QVBoxLayout* m_root_vbox;
    QTreeView* m_config_groups_tree;
    QWidget* m_config_items_container;
    QStandardItemModel* m_tree_view_model;
    QPushButton* m_ok_btn;
    QPushButton* m_cancel_btn;
    QPushButton* m_apply_btn;
};

}

Q_DECLARE_METATYPE(yasem::ConfigContainerHelper)

#endif // SETTINGSDIALOG_H
