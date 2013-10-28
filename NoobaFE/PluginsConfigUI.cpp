#include <QMessageBox>

#include "PluginsConfigUI.h"
#include "NoobaPlugin.h"

PluginsConfigUI::PluginsConfigUI(PluginLoader& pluginLoader, QWidget *parent)
    : QDialog(parent),
      _pluginLoader(pluginLoader),
      _pluginConnDelegate(new PluginConnDelegate(&pluginLoader,this)),
      ACTIVE(tr("Active")),
      DISABLED(tr("Disabled"))
{
    FILENAME_ROLE = Qt::UserRole;
    ALIAS_ROLE = Qt::UserRole +2;
    ui.setupUi(this);
    updateUI();
    connect(&_pluginLoader, SIGNAL(pluginsDisconnected(PluginLoader::PluginConnData*)), this,
            SLOT(onPluginsDisconnected(PluginLoader::PluginConnData*)));
}

PluginsConfigUI::~PluginsConfigUI()
{
}

void PluginsConfigUI::onReloadButtonClicked()
{
    _pluginLoader.loadPluginInfo();
    _pluginLoader.unloadPlugins();
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
    if(_pluginLoader.loadPlugin(var.toString()))
    {
        item->setText(1, ACTIVE);
    }
}

void PluginsConfigUI::updateUI()
{
    ui.pluginListTree->clear();
    ui.pluginListTree->setColumnCount(2);
    ui.pluginListTree->setHeaderLabels(QStringList() << tr("Property") << tr("Value"));
    ui.pluginConfigTree->setItemDelegate(_pluginConnDelegate);

    foreach(const nooba::PluginData& pluginData, _pluginLoader.getPluginInfo())
    {
        QTreeWidgetItem* level0 = new QTreeWidgetItem(0);
        QString pluginName = pluginData._pluginInfo.name();
        level0->setText(0, pluginName);
        level0->setData(0, FILENAME_ROLE, pluginData._fileName); // used to set the default plugin

        // Item in available plugins column
        QTreeWidgetItem* l1 = new QTreeWidgetItem(1);
        l1->setText(0, pluginName);
        l1->setData(0, FILENAME_ROLE, pluginData._fileName); // used to identify the plugin.
        l1->setToolTip(0, tr("Plugin: <b>%1</b>\nFileName: <b>%2</b>" ).arg(pluginData._pluginInfo.name()).arg(pluginData._fileName));
        ui.availablePluginsTree->addTopLevelItem(l1);

        // TODO: Need to check the loaded plugins and update the UI
        // sort the available and loaded list.. then iterate thru the available list while checkinavailability
        // of the loaded plugins

//        if(m_pluginLoader.getActivePluginFilename().compare(pluginData._fileName) == 0) // if plugin available
//        {
//            level0->setText(1, ACTIVE);
//        }
//        else
//        {
//            level0->setText(1, DISABLED);
//        }

        ui.pluginListTree->addTopLevelItem(level0);

        QTreeWidgetItem* level1 = new QTreeWidgetItem(1);
        level1->setText(0, tr("Version"));
        QString str(tr("%1.%2").arg(pluginData._pluginInfo.majorVersion())
                    .arg(pluginData._pluginInfo.majorVersion()));
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
        str = tr("%1").arg(pluginData._pluginInfo.description());
        level1->setText(1, str);
        level1->setToolTip(1, str);
        level0->addChild(level1);

        level1 = new QTreeWidgetItem(1);
        level1->setText(0, tr("Created by"));
        str = tr("%1").arg(pluginData._pluginInfo.author());
        level1->setText(1, str);
        level1->setToolTip(1, str);
        level0->addChild(level1);

        int count = ui.pluginListTree->columnCount();
        for(int i = 0; i < count; ++i)
            ui.pluginListTree->resizeColumnToContents(i);
    }
    foreach (NoobaPlugin* p, _pluginLoader.getActivePlugins()) {
        addLoadedPluginToTree(p);
    }
}


void PluginsConfigUI::on_addButton_clicked()
{
    QTreeWidgetItem* itm = new QTreeWidgetItem(1);
    itm->setFlags(itm->flags()| Qt::ItemIsEditable);
    ui.pluginConfigTree->addTopLevelItem(itm);
}

void PluginsConfigUI::on_removeButton_clicked()
{
    QList<QTreeWidgetItem* > lst =  ui.pluginConfigTree->selectedItems();
    if(lst.isEmpty())
    {
        QMessageBox errMsg;
        errMsg.setText(tr("First select an item to be removed"));
        errMsg.exec();
        return;
    }
    delete lst.first();
}

void PluginsConfigUI::on_applyButton_clicked()
{
    QList<PluginLoader::PluginConnData* > lst;
    for(int r=0; r < ui.pluginConfigTree->topLevelItemCount(); r++)
    {
        QTreeWidgetItem* itm = ui.pluginConfigTree->topLevelItem(r);
        if(!itm)
            continue;
        if(itm->text(0).isEmpty() || itm->text(1).isEmpty())    // empty line
            continue;
        PluginLoader::PluginConnData* connData = new PluginLoader::PluginConnData;
        QVariant v = itm->data(0, Qt::UserRole);
        NoobaPlugin* p = v.value<NoobaPlugin* >();
        connData->_outPlug = p;
        v = itm->data(1, Qt::UserRole);
        p = v.value<NoobaPlugin* >();
        connData->_inPlug = p;

        lst.append(connData);
    }
    _pluginLoader.connectAllPlugins(lst);
}

void PluginsConfigUI::on_loadPluginButton_clicked()
{
    QList<QTreeWidgetItem* > l = ui.availablePluginsTree->selectedItems();
    if(l.isEmpty())
    {
        QMessageBox errMsg;
        errMsg.setText(tr("Select a plugin to load"));
        errMsg.exec();
        return;
    }

    // TODO: Need to check for the already loaded plugin of the same type and rename it.
    //       This should be consistent with other parts.
    QTreeWidgetItem* itm = l.first();

    bool isBase;
    (ui.loadedPluginsTree->topLevelItemCount() == 0) ? isBase =true: isBase = false;
    NoobaPlugin* p = _pluginLoader.loadPlugin(itm->data(0, FILENAME_ROLE).toString(), isBase);
    addLoadedPluginToTree(p);

}

void PluginsConfigUI::addLoadedPluginToTree(NoobaPlugin *p)
{
    QTreeWidgetItem* pluginItm = new QTreeWidgetItem(ui.loadedPluginsTree);
    pluginItm->setData(0, ALIAS_ROLE, QVariant(p->alias()));
    if(p->isBasePlugin())
    {
        pluginItm->setText(0, QString("[Base]").append(p->alias()));
        pluginItm->setForeground(0, QBrush(QColor(155,120,0)));
    }
    else
    {
        pluginItm->setText(0, p->alias());
    }
    pluginItm->setData(0, FILENAME_ROLE, p->fileName()); // used to identify the plugin.
    pluginItm->setToolTip(0, tr("Plugin: <b>%1</b>\nFileName: <b>%2</b>" ).arg(p->getPluginInfo().name()).arg(p->fileName()));
}

void PluginsConfigUI::on_unloadPluginButton_clicked()
{
    QList<QTreeWidgetItem* > l = ui.loadedPluginsTree->selectedItems();
    if(l.isEmpty())
    {
        QMessageBox errMsg;
        errMsg.setText(tr("Select a plugin to unload"));
        errMsg.exec();
        return;
    }

    _pluginLoader.unloadPlugin(l.first()->data(0, ALIAS_ROLE).toString());
    delete l.first();
    return;
}

void PluginsConfigUI::on_cancelButton_clicked()
{

}

void PluginsConfigUI::onPluginsDisconnected(PluginLoader::PluginConnData *pcd)
{
    for(int r = 0; r < ui.pluginConfigTree->topLevelItemCount(); r++)
    {
        QTreeWidgetItem* itm = ui.pluginConfigTree->topLevelItem(r);
        if(itm->text(0).compare(pcd->_outPlugAlias) == 0 && itm->text(1).compare(pcd->_inPlugAlias) == 0)
        {
            delete itm;
        }
    }
}
