#include "PluginsConfigUI.h"
#include "PluginLoader.h"

PluginsConfigUI::PluginsConfigUI(PluginLoader& pluginLoader, QWidget *parent)
    : QDialog(parent),
    m_pluginLoader(pluginLoader),
    ACTIVE(tr("Active")),
    DISABLED(tr("Disabled"))
{
    ui.setupUi(this);
    updateUI();
}

PluginsConfigUI::~PluginsConfigUI()
{

}

void PluginsConfigUI::onReloadButtonClicked()
{
    m_pluginLoader.loadPluginInfo();
    m_pluginLoader.unloadActivePlugin();
    updateUI();
}

void PluginsConfigUI::onSetDefaultButtonClicked()
{
    QTreeWidgetItem* item = ui.pluginListTree->currentItem();
    if(!item)
        return;

    if(item->type() != 0)
    {
        item = item->parent();
    }

    // set previous active plugin label to disabled
    QList<QTreeWidgetItem* > itemList = ui.pluginListTree->findItems(ACTIVE, Qt::MatchExactly, 1);
    foreach(QTreeWidgetItem* i, itemList)
    {
        i->setText(1, DISABLED);
    }

    QVariant var = item->data(0, Qt::UserRole+1);
    if(m_pluginLoader.loadPlugin(var.toString()))
    {
        item->setText(1, ACTIVE);
    }
}

void PluginsConfigUI::updateUI()
{
    ui.pluginListTree->clear();
    ui.pluginListTree->setColumnCount(2);
    ui.pluginListTree->setHeaderLabels(QStringList() << tr("Property") << tr("Value"));

    foreach(const nooba::PluginData& pluginData, m_pluginLoader.getPluginInfo())
    {
        QTreeWidgetItem* level0 = new QTreeWidgetItem(0);
        QString pluginName = pluginData._pluginInfo._name;
        level0->setText(0, pluginName);
        level0->setData(0, Qt::UserRole+1, pluginData._fileName); // used to set the default plugin

        if(m_pluginLoader.getActivePlugin()) // if plugin available
        {
            level0->setText(1, ACTIVE);
        }
        else
        {
            level0->setText(1, DISABLED);
        }

        ui.pluginListTree->addTopLevelItem(level0);

        QTreeWidgetItem* level1 = new QTreeWidgetItem(1);
        level1->setText(0, tr("Version"));
        QString str(tr("%1.%2").arg(pluginData._pluginInfo._majorVersion)
                    .arg(pluginData._pluginInfo._majorVersion));
        level1->setText(1, str);
        level1->setToolTip(1, str);
        level0->addChild(level1);

        level1 = new QTreeWidgetItem(1);
        level1->setText(0, tr("FileName"));
        str = pluginData._fileName;
        level1->setText(1, str );
        level1->setToolTip(1, str);
        level0->addChild(level1);

        level1 = new QTreeWidgetItem(1);
        level1->setText(0, tr("Description"));
        str = tr("%1").arg(pluginData._pluginInfo._description);
        level1->setText(1, str);
        level1->setToolTip(1, str);
        level0->addChild(level1);

        level1 = new QTreeWidgetItem(1);
        level1->setText(0, tr("Created by"));
        str = tr("%1").arg(pluginData._pluginInfo._author);
        level1->setText(1, str);
        level1->setToolTip(1, str);
        level0->addChild(level1);

        int count = ui.pluginListTree->columnCount();
        for(int i = 0; i < count; ++i)
            ui.pluginListTree->resizeColumnToContents(i);
        
    }
}
