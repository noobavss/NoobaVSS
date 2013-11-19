#include <QMessageBox>
#include <QDebug>
#include <QApplication>

#include "PluginsConfigUI.h"
#include "NoobaPlugin.h"

PluginsConfigUI::PluginsConfigUI(PluginLoader& pluginLoader, QWidget *parent)
    : QDialog(parent),
      _pluginLoader(pluginLoader),
      _pluginConnDelegate(NULL),
      ACTIVE(tr("Active")),
      DISABLED(tr("Disabled"))
{
    _pluginConnDelegate = new PluginConnDelegate(this);
    FILENAME_ROLE = Qt::UserRole;
    ALIAS_ROLE = Qt::UserRole +2;
    ui.setupUi(this);
    updateUI();
    ui.pluginConfigTree->setColumnWidth(0, 307);

    connect(this, SIGNAL(loadPlugins(QStringList)), &_pluginLoader, SLOT(loadPlugins(QStringList)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(loadPluginInfo()), &_pluginLoader, SLOT(loadPluginInfo()), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(saveCurrentConfig()), &_pluginLoader, SLOT(saveCurrentConfig()), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(unloadAllPlugins()), &_pluginLoader, SLOT(unloadPlugins()), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(connectPlugins(QStringList,QStringList)), &_pluginLoader, SLOT(connectPlugins(QStringList,QStringList)), Qt::BlockingQueuedConnection);
}

PluginsConfigUI::~PluginsConfigUI()
{
}

QStringList PluginsConfigUI::getOutputPluginList(const QString &inPlugAlias)
{
    Q_UNUSED(inPlugAlias)
    // TODO update the list according to inPlug

    QStringList lst;
    for(int i = 0; i < ui.loadedPluginsTree->topLevelItemCount(); i++)
    {
        lst << ui.loadedPluginsTree->topLevelItem(i)->text(0);
    }

    return lst;
}

QStringList PluginsConfigUI::getInputPluginList(const QString &outPlugAlias)
{
    Q_UNUSED(outPlugAlias)
    // TODO update the list according to outplug
    QStringList lst;
    for(int i = 0; i < ui.loadedPluginsTree->topLevelItemCount(); i++)
    {
        lst << ui.loadedPluginsTree->topLevelItem(i)->text(0);
    }

    return lst;
}

void PluginsConfigUI::onReloadButtonClicked()
{
    emit loadPluginInfo();
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
    ui.availablePluginsTree->clear();
    ui.pluginListTree->setColumnCount(2);
    ui.pluginListTree->setHeaderLabels(QStringList() << tr("Property") << tr("Value"));

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
        // sort the available and loaded list.. then iterate thru the available list while checkin availability
        // of the loaded plugins
//        foreach(NoobaPlugin* p, _pluginLoader.getActivePlugins())
//        {
//            if(p->fileName().compare(pluginData._fileName) == 0) // if plugin available
//            {
//                level0->setText(1, ACTIVE);
//            }
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

    // update connection table
    QList<PluginConnData* > lst = _pluginLoader.getPCDList();

    foreach(PluginConnData* pcd, lst)
    {
        QTreeWidgetItem* itm = new QTreeWidgetItem(ui.pluginConfigTree);
        itm->setFlags(itm->flags()| Qt::ItemIsEditable);
        QColor c(190, 190,240);
        itm->setBackgroundColor(0, c);
        itm->setBackground(1, c);
        itm->setText(0, pcd->_outPlugAlias);
    }
    ui.pluginConfigTree->setItemDelegate(_pluginConnDelegate);
}

void PluginsConfigUI::on_addButton_clicked()
{
    QTreeWidgetItem* itm = new QTreeWidgetItem(1);
    itm->setFlags(itm->flags()| Qt::ItemIsEditable);
    itm->setBackgroundColor(0, QColor(240, 240,240));
    itm->setBackground(1, QColor(240, 240,240));
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
    emit unloadAllPlugins();
    if(ui.loadedPluginsTree->topLevelItemCount() == 0)
    {
        emit saveCurrentConfig();
        return;
    }

    QStringList fNameList;
    for(int i = 0; i < ui.loadedPluginsTree->topLevelItemCount();  i++)
    {
        fNameList.append(ui.loadedPluginsTree->topLevelItem(i)->data(0,FILENAME_ROLE).toString());
    }

    if(!fNameList.isEmpty())
    {
        emit loadPlugins(fNameList);

        QStringList in, out;
        for(int r=0; r < ui.pluginConfigTree->topLevelItemCount(); r++)
        {
            QTreeWidgetItem* itm = ui.pluginConfigTree->topLevelItem(r);

            if(itm->text(0).isEmpty() || itm->text(1).isEmpty())    // empty line
                continue;

            out << itm->text(0);
            in << itm->text(1);
        }
        emit connectPlugins(out, in);
    }
    emit saveCurrentConfig();
//    QThread::msleep(200);
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
    MoveTreeWidgetItem(ui.availablePluginsTree, ui.loadedPluginsTree, l.first());
}

void PluginsConfigUI::addLoadedPluginToTree(NoobaPlugin *p)
{
    QList<QTreeWidgetItem* > lst = ui.availablePluginsTree->findItems(p->alias(), Qt::MatchExactly, 0);

    if(lst.isEmpty())
    {
        qDebug() << tr("plugin [ %1 ] not found in available plugins list").arg(p->alias()) << Q_FUNC_INFO ;
        return;
    }

    MoveTreeWidgetItem(ui.availablePluginsTree, ui.loadedPluginsTree, lst.first());
}

void PluginsConfigUI::MoveTreeWidgetItem(QTreeWidget *from, QTreeWidget *to, QTreeWidgetItem* item)
{
    from->takeTopLevelItem(from->indexOfTopLevelItem(item));
    to->addTopLevelItem(item);
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
    MoveTreeWidgetItem(ui.loadedPluginsTree, ui.availablePluginsTree, l.first());
    removeConnections(l.first()->text(0));
}

void PluginsConfigUI::on_cancelButton_clicked()
{
    close();
}

//void PluginsConfigUI::onPluginsDisconnected(PluginConnData *pcd)
//{
//    for(int r = 0; r < ui.pluginConfigTree->topLevelItemCount(); r++)
//    {
//        QTreeWidgetItem* itm = ui.pluginConfigTree->topLevelItem(r);
//        if(itm->text(0).compare(pcd->_outPlugAlias) == 0 && itm->text(1).compare(pcd->_inPlugAlias) == 0)
//        {
//            delete itm;
//        }
//    }
//}

void PluginsConfigUI::closeEvent(QCloseEvent *)
{
   // _pluginLoader.loadPrevConfig(); // this is not reverting back :/ this may lead to loosing the states of plugins
                                    // this reloads the plugins to its previous state.
    return;
}

void PluginsConfigUI::on_doneButton_clicked()
{
 //   on_applyButton_clicked();
    close();
}

void PluginsConfigUI::removeConnections(const QString &pluginAlias)
{
    int i = ui.pluginConfigTree->topLevelItemCount() - 1;
    for(; i >= 0; i--)
    {
        QTreeWidgetItem* itm = ui.pluginConfigTree->topLevelItem(i);
        if(itm->text(0).compare(pluginAlias) == 0 || itm->text(1).compare(pluginAlias) == 0 )
        {
            delete itm;
        }
    }
}
