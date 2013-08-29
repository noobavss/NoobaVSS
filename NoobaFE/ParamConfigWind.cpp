#include "ParamConfigWind.h"
#include "ui_ParamConfigWind.h"
#include "NoobaPlugin.h"
#include "ParamDelegate.h"
#include "NoobaEye.h"

#include <QTreeWidget>
#include <QModelIndex>
#include <QScopedPointer>
#include <noobapluginapi.h>

ParamConfigWind::ParamConfigWind(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParamConfigWind),
    _delegateSP(new ParamDelegate),
    _noEditDelegateSP(new NoEditDelegate)

{
    ui->setupUi(this);
    connect(ui->paramTree, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this, SLOT(onItemChanged(QTreeWidgetItem*,int)));

    ui->paramTree->setItemDelegateForColumn(0, _noEditDelegateSP.data());
    ui->paramTree->setItemDelegateForColumn(1, _delegateSP.data());

//    ui->paramTree->show();
}

ParamConfigWind::~ParamConfigWind()
{
    delete ui;
}

void ParamConfigWind::setPlugin(NoobaPlugin *plugin)
{
    ui->paramTree->clear();
    if(!plugin)
    {
        return;
    }
    _plugin = plugin;
    _delegateSP.data()->setPlugin(plugin);
    addToTree<const QMap<QString, IntData* >, nooba::ParamType>(plugin->getIntParamMap(), nooba::IntParam);
    addToTree<const QMap<QString, DoubleData* >, nooba::ParamType>(plugin->getDoubleParamMap(), nooba::DoubleParam);
    addToTree<const QMap<QString, StringData* >, nooba::ParamType>(plugin->getStringMap(), nooba::StringParam);
}

void ParamConfigWind::onItemChanged(QTreeWidgetItem* item, int  column)
{
    nooba::ParamType type = item->data(column, nooba::ParamTypeRole).value<nooba::ParamType>();
    QString varName = item->data(0, Qt::DisplayRole).toString();

    switch(type)
    {
    case nooba::IntParam:
    {
        _plugin->onIntParamUpdate(varName, item->data(1, Qt::DisplayRole).toInt());
        break;
    }
    case nooba::DoubleParam:
    {
        _plugin->onDoubleParamUpdate(varName, item->data(1, Qt::DisplayRole).toDouble());
        break;
    }
    case nooba::StringParam:
    {
        _plugin->onStringParamUpdate(varName, item->text(column));
        break;
    }
    default:
        return;
    }
    return;
}

template<typename Map, typename Type>
void ParamConfigWind::addToTree(Map& map, Type type)
{
    typename Map::ConstIterator itr;           // typename needed for nested types
    for(itr = map.constBegin(); itr != map.constEnd(); itr++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(2);
        item->setText(0, itr.key());
        item->setText(1, QString("%1").arg(itr.value()->_val));

        QVariant var = qVariantFromValue(type);
        item->setData(1, nooba::ParamTypeRole, var);
        var = qVariantFromValue(itr.value());
        item->setData(1, nooba::ParamDataRole, var);
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable | Qt::ItemIsEnabled);
        ui->paramTree->addTopLevelItem(item);
    }
}
