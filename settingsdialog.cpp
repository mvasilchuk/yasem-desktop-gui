#include "settingsdialog.h"

#include "yasemsettings.h"
#include "core.h"
#include "macros.h"

#include <QStandardItemModel>
#include <QHeaderView>
#include <QLineEdit>
#include <QStandardItem>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>

using namespace yasem;

static const char* USER_DATA_PROP_KEY = "key";

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_settings(Core::instance()->yasem_settings()),
    m_root_vbox(new QVBoxLayout(this)),
    m_config_groups_tree(new QTreeView(this)),
    m_config_items_container(new QWidget(this)),
    m_tree_view_model(new QStandardItemModel()),

    m_ok_btn(new QPushButton(this)),
    m_cancel_btn(new QPushButton(this)),
    m_apply_btn(new QPushButton(this))
{
    setObjectName("Settings dialog");
    setupDialog();

    /*
    ConfigTreeGroup* testGroup = new ConfigTreeGroup("test", "test_group", tr("Test group"));
    ConfigItem* testInputText = new ConfigItem("test_key_string", tr("Test string input"), "0", ConfigItem::STRING);
    ConfigItem* testInputBool = new ConfigItem("test_key_bool", tr("Test string bool"), QVariant(false), ConfigItem::BOOL);
    ConfigItem* testInputNumber = new ConfigItem("test_key_number", tr("Test string int"), 0, ConfigItem::INT);

    ConfigItem* testInputList = new ConfigItem("test_key_list", tr("Test string string list"), "", ConfigItem::LIST);
    testInputList->options().insert("test option 1", "value 1");
    testInputList->options().insert("test option 2", "value 2");
    testInputList->options().insert("test option 3", "value 3");

    testGroup->addItem(testInputText);
    testGroup->addItem(testInputBool);
    testGroup->addItem(testInputNumber);
    testGroup->addItem(testInputList);


    m_settings->getDefaultGroup(YasemSettings::APPEARANCE)->addItem(testGroup);*/

    updateConfigGroups();
}

void SettingsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            break;
        default:
            break;
    }
}

void SettingsDialog::setupDialog()
{
    setWindowTitle(tr("Settings"));

    QHBoxLayout* hbox_layout = new QHBoxLayout();

    m_config_groups_tree->setObjectName("Config groups tree");
    m_config_groups_tree->setMinimumSize(150, 400);
    m_config_groups_tree->setMaximumWidth(300);
    m_config_groups_tree->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_config_groups_tree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_config_groups_tree->setModel(m_tree_view_model);
    m_config_groups_tree->header()->hide();
    connect(m_config_groups_tree, &QTreeView::activated, this, &SettingsDialog::updateConfigPage);

    hbox_layout->addWidget(m_config_groups_tree);

    m_config_items_container->setObjectName("Config items container");
    m_config_items_container->setMinimumSize(300, 400);
    m_config_items_container->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    m_config_items_container->setLayout(new QGridLayout());

    hbox_layout->addWidget(m_config_items_container);


    QHBoxLayout* buttons_layout = new QHBoxLayout();

    buttons_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

    m_ok_btn->setText(tr("OK"));
    m_cancel_btn->setText(tr("Cancel"));
    m_apply_btn->setText(tr("Apply"));

    buttons_layout->addWidget(m_ok_btn);
    buttons_layout->addWidget(m_cancel_btn);
    buttons_layout->addWidget(m_apply_btn);

    connect(m_ok_btn, &QPushButton::clicked, [=]() {
        m_settings->save();
        close();
    });
    connect(m_cancel_btn, &QPushButton::clicked, this, &SettingsDialog::cancel);
    connect(m_apply_btn, &QPushButton::clicked, [=]() { m_settings->save(); });

    m_root_vbox->addItem(hbox_layout);
    m_root_vbox->addItem(buttons_layout);

    setLayout(m_root_vbox);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    //resize(minimumSize());
}

void SettingsDialog::updateConfigGroups()
{
    QHash<const QString&, ConfigTreeGroup*> groups = m_settings->getConfigGroups();

    m_tree_view_model->clear();
    for(ConfigTreeGroup* group: groups)
    {
        m_tree_view_model->appendRow(addTreeItem(group));
    }
    m_tree_view_model->sort(0);
}

QStandardItem *SettingsDialog::addTreeItem(ConfigTreeGroup *group)
{
    Q_ASSERT(group != NULL);
    QStandardItem* row_item = new QStandardItem(group->getTitle());
    row_item->setData(QVariant::fromValue(group));
    DEBUG() << "setData" << QVariant::fromValue(group) << group->getTitle();
    row_item->setEditable(false);

    for(ConfigItem* item: group->getItems())
    {
        if(item->isContainer())
        {
            ConfigContainer* container = dynamic_cast<ConfigContainer*>(item);
            Q_ASSERT(container != NULL);
            if(container->getContainerType() == ConfigContainer::CONFIG_GROUP)
                row_item->appendRow(addTreeItem(static_cast<ConfigTreeGroup*>(container)));
        }
    }
    return row_item;
}

void SettingsDialog::updateConfigPage(const QModelIndex &index)
{
    DEBUG() << "updateConfigPage";
    QGridLayout* layout = dynamic_cast<QGridLayout*>(m_config_items_container->layout());
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != 0)
    {
        delete child->widget();
        delete child;
    }

    QVariant data = m_tree_view_model->itemFromIndex(index)->data();

    ConfigContainer* container = dynamic_cast<ConfigContainer*>(data.value<QObject*>());

    if(container == NULL)
    {
        WARN() << "Configuration container not found: " << index;
        return;
    }

    m_settings->load(container);

    int row_index = 0;
    for(ConfigItem* item: container->getItems())
    {
        switch(item->getType())
        {
            case ConfigItem::STRING:
            case ConfigItem::INT:
            case ConfigItem::LONG:
            case ConfigItem::DOUBLE:
            {
                QLabel* label = new QLabel(item->getTitle());
                layout->addWidget(label, row_index, 0);

                QLineEdit* line_edit = new QLineEdit(m_config_items_container);
                line_edit->setText(item->getValue().toString());

                switch(item->getType())
                {
                    case ConfigItem::INT: {
                        line_edit->setValidator( new QIntValidator(this) );
                        break;
                    }
                    case ConfigItem::LONG: {
                        line_edit->setValidator( new QIntValidator(this) );
                        break;
                    }
                    case ConfigItem::DOUBLE: {
                        line_edit->setValidator( new QDoubleValidator(this) );
                        break;
                    }
                }

                line_edit->setProperty(USER_DATA_PROP_KEY, QVariant::fromValue(item));
                connect(line_edit, &QLineEdit::textEdited, this, &SettingsDialog::onConfigDataTextChanged);
                layout->addWidget(line_edit, row_index, 1);
                break;
            }
            case ConfigItem::BOOL: {
                QLabel* label = new QLabel(item->getTitle());
                layout->addWidget(label, row_index, 0);

                QCheckBox* check_box = new QCheckBox(m_config_items_container);
                check_box->setChecked(item->getValue().toBool());
                check_box->setProperty(USER_DATA_PROP_KEY, QVariant::fromValue(item));
                connect(check_box, &QCheckBox::toggled, this, &SettingsDialog::onConfigDataBoolChanged);
                layout->addWidget(check_box, row_index, 1);
                break;
            }
            case ConfigItem::LIST: {
                QLabel* label = new QLabel(item->getTitle());
                layout->addWidget(label, row_index, 0);

                QComboBox* combo_box = new QComboBox(m_config_items_container);
                ListConfigItem* list_item = dynamic_cast<ListConfigItem*>(item);
                for(const QString& title: list_item->options().keys())
                {
                    QString value = list_item->options().value(title).toString();
                    DEBUG() << "option:" << title << value;
                    combo_box->addItem(title, value);
                }
                combo_box->setCurrentIndex(combo_box->findData(item->getValue()));
                combo_box->setProperty(USER_DATA_PROP_KEY, QVariant::fromValue(item));
                connect(combo_box, SIGNAL(currentIndexChanged(int)), this, SLOT(onConfigDataListItemChanged(int)));
                layout->addWidget(combo_box, row_index, 1);
                break;
            }
            default: {
                WARN() << "item type not found!" << item->getTitle() << item->getType();
            }
        }
        row_index++;
    }

    QSpacerItem* spacer = new QSpacerItem(0, 0,QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout->addItem(spacer, row_index, 0);
}

void SettingsDialog::onConfigDataTextChanged(const QString &text)
{
    ConfigItem* item = getItemFromObject(sender());
    if(item == NULL) return;

    DEBUG() << item->getTitle() << "changed" << text;
    item->setValue(text);
    item->setDirty(true);
}

void SettingsDialog::onConfigDataBoolChanged(bool checked)
{
    ConfigItem* item = getItemFromObject(sender());
    if(item == NULL) return;

    DEBUG() << item->getTitle() << "changed" << checked;
    item->setValue(checked);
    item->setDirty(true);
}

void SettingsDialog::onConfigDataListItemChanged(int index)
{
    ConfigItem* item = getItemFromObject(sender());
    if(item == NULL) return;
    QComboBox* combo_box = qobject_cast<QComboBox*>(sender());

    DEBUG() << item->getTitle() << "changed" << index << combo_box->itemData(index);

    item->setValue(combo_box->itemData(index));
    item->setDirty(true);
}

void SettingsDialog::cancel()
{
    m_settings->reset();
    close();
}

ConfigItem* SettingsDialog::getItemFromObject(QObject *object)
{
    QWidget* widget = qobject_cast<QWidget*>(object);

    ConfigItem* config_item = dynamic_cast<ConfigItem*>(widget->property(USER_DATA_PROP_KEY).value<QObject*>());
    if(config_item == NULL)
    {
        WARN() << "No configuration item is set to object" << widget;
        return NULL;
    }
    return config_item;
}



