#include "settingsdialog.h"

#include "config.h"
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
    m_settings(SDK::Core::instance()->yasem_settings()),
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
    QHash<const QString&, SDK::ConfigTreeGroup*> groups = m_settings->getConfigGroups();

    m_tree_view_model->clear();
    for(SDK::ConfigTreeGroup* group: groups)
    {
        m_tree_view_model->appendRow(addTreeItem(group));
    }
    m_tree_view_model->sort(0);
}

QStandardItem *SettingsDialog::addTreeItem(SDK::ConfigTreeGroup *group)
{
    Q_ASSERT(group != NULL);
    QString title = group->getTitle();
    QStandardItem* row_item = new QStandardItem(group->isEnabled() ? title : title.append(tr(" (not active)")));
    row_item->setEnabled(group->isEnabled());
    ConfigContainerHelper helper;
    helper.container = group;
    row_item->setData(QVariant::fromValue(helper));
    DEBUG() << "setData" << QVariant::fromValue(group) << group->getTitle();
    row_item->setEditable(false);

    if(group->isEnabled())
    {
        for(SDK::ConfigItem* item: group->getItems())
        {
            DEBUG() << "Subitem" << item << item->getKey() << item->isContainer();
            if(item->isContainer())
            {
                SDK::ConfigContainer* container = dynamic_cast<SDK::ConfigContainer*>(item);
                Q_ASSERT(container);
                if(container->getContainerType() == SDK::ConfigContainer::CONFIG_GROUP)
                {
                    SDK::ConfigTreeGroup* tree_group = dynamic_cast<SDK::ConfigTreeGroup*>(container);
                    row_item->appendRow(addTreeItem(tree_group));
                }
            }
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
    SDK::ConfigContainer* container = dynamic_cast<SDK::ConfigContainer*>(data.value<ConfigContainerHelper>().container);

    if(container == NULL)
    {
        WARN() << "Configuration container not found: " << index;
        return;
    }

    m_settings->load(container);

    int row_index = 0;
    for(SDK::ConfigItem* item: container->getItems())
    {
        switch(item->getType())
        {
            case SDK::ConfigItem::STRING:
            case SDK::ConfigItem::INT:
            case SDK::ConfigItem::LONG:
            case SDK::ConfigItem::DOUBLE:
            {
                QLabel* label = new QLabel(item->getTitle());
                label->setEnabled(item->isEnabled());
                layout->addWidget(label, row_index, 0);

                QLineEdit* line_edit = new QLineEdit(m_config_items_container);
                line_edit->setText(item->getValue().toString());

                switch(item->getType())
                {
                    case SDK::ConfigItem::INT: {
                        line_edit->setValidator( new QIntValidator(this) );
                        break;
                    }
                    case SDK::ConfigItem::LONG: {
                        line_edit->setValidator( new QIntValidator(this) );
                        break;
                    }
                    case SDK::ConfigItem::DOUBLE: {
                        line_edit->setValidator( new QDoubleValidator(this) );
                        break;
                    }
                }

                line_edit->setProperty(USER_DATA_PROP_KEY, QVariant::fromValue(item));
                line_edit->setEnabled(item->isEnabled());
                connect(line_edit, &QLineEdit::textEdited, this, &SettingsDialog::onConfigDataTextChanged);
                layout->addWidget(line_edit, row_index, 1);
                break;
            }
            case SDK::ConfigItem::BOOL: {
                QLabel* label = new QLabel(item->getTitle());
                label->setEnabled(item->isEnabled());
                layout->addWidget(label, row_index, 0);

                QCheckBox* check_box = new QCheckBox(m_config_items_container);
                check_box->setChecked(item->getValue().toBool());
                check_box->setProperty(USER_DATA_PROP_KEY, QVariant::fromValue(item));
                check_box->setEnabled(item->isEnabled());
                connect(check_box, &QCheckBox::toggled, this, &SettingsDialog::onConfigDataBoolChanged);
                layout->addWidget(check_box, row_index, 1);
                break;
            }
            case SDK::ConfigItem::LIST: {
                QLabel* label = new QLabel(item->getTitle());
                label->setEnabled(item->isEnabled());
                layout->addWidget(label, row_index, 0);

                QComboBox* combo_box = new QComboBox(m_config_items_container);
                SDK::ListConfigItem* list_item = dynamic_cast<SDK::ListConfigItem*>(item);
                for(const QString& title: list_item->options().keys())
                {
                    QString value = list_item->options().value(title).toString();
                    DEBUG() << "option:" << title << value;
                    combo_box->addItem(title, value);
                }
                combo_box->setCurrentIndex(combo_box->findData(item->getValue()));
                combo_box->setProperty(USER_DATA_PROP_KEY, QVariant::fromValue(item));
                combo_box->setEnabled(item->isEnabled());
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
    SDK::ConfigItem* item = getItemFromObject(sender());
    if(item == NULL) return;

    DEBUG() << item->getTitle() << "changed" << text;
    item->setValue(text);
    item->setDirty(true);
}

void SettingsDialog::onConfigDataBoolChanged(bool checked)
{
    SDK::ConfigItem* item = getItemFromObject(sender());
    if(item == NULL) return;

    DEBUG() << item->getTitle() << "changed" << checked;
    item->setValue(checked);
    item->setDirty(true);
}

void SettingsDialog::onConfigDataListItemChanged(int index)
{
    SDK::ConfigItem* item = getItemFromObject(sender());
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

SDK::ConfigItem* SettingsDialog::getItemFromObject(QObject *object)
{
    QWidget* widget = qobject_cast<QWidget*>(object);

    SDK::ConfigItem* config_item = dynamic_cast<SDK::ConfigItem*>(widget->property(USER_DATA_PROP_KEY).value<QObject*>());
    if(config_item == NULL)
    {
        WARN() << "No configuration item is set to object" << widget;
        return NULL;
    }
    return config_item;
}



