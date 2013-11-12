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
    PluginPtrRole = Qt::UserRole+1;
    connect(ui->paramTree, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this, SLOT(onItemChanged(QTreeWidgetItem*,int)));

    ui->paramTree->setItemDelegateForColumn(0, _noEditDelegateSP.data());
    ui->paramTree->setItemDelegateForColumn(1, _delegateSP.data());
}

ParamConfigWind::~ParamConfigWind()
{
    delete ui;
}

void ParamConfigWind::addPlugin(NoobaPlugin *plugin)
{
    if(!plugin)
    {
        return;
    }

    QTreeWidgetItem* l1 = new QTreeWidgetItem(1);
    l1->setText(0, plugin->alias());
    QVariant v;
    v.setValue(plugin);
    l1->setData(0, PluginPtrRole, v);
    addToTree<const QMap<QString, IntData* >, nooba::ParamType>(plugin->getIntParamMap(), nooba::IntParam, l1);
    addToTree<const QMap<QString, DoubleData* >, nooba::ParamType>(plugin->getDoubleParamMap(), nooba::DoubleParam, l1);
    addToTree<const QMap<QString, StringData* >, nooba::ParamType>(plugin->getStringMap(), nooba::StringParam, l1);
    addToTree<const QMap<QString, StringListData* >, nooba::ParamType>(plugin->getStringListMap(), nooba::MultiValueParam, l1);

    l1->setBackground(0, QBrush(QColor(150,150,150)));
    l1->setForeground(0, QBrush(QColor(255,255,255)));
    QFont font;
    font.setBold(true);
    l1->setFont(0, font);
    ui->paramTree->addTopLevelItem(l1);
    l1->setFirstColumnSpanned(true);
    l1->setExpanded(true);

    return;
}

void ParamConfigWind::removePlugin(NoobaPlugin* plugin)
{
    for(int i = 0; i < ui->paramTree->topLevelItemCount(); i++)
    {
        if(ui->paramTree->topLevelItem(i)->text(0).compare(plugin->alias()) == 0)
        {
            delete ui->paramTree->topLevelItem(i);
        }
    }
}

void ParamConfigWind::clear()
{
    ui->paramTree->clear();
}

void ParamConfigWind::onItemChanged(QTreeWidgetItem* item, int  column)
{
    nooba::ParamType type = item->data(column, nooba::ParamTypeRole).value<nooba::ParamType>();
    QString varName = item->data(0, Qt::DisplayRole).toString();

    QVariant v = item->parent()->data(0, PluginPtrRole);
    if(!v.isValid())
        return;

    NoobaPlugin* p = v.value<NoobaPlugin*>();

    switch(type)
    {
    case nooba::IntParam:
    {
        p->onIntParamUpdate(varName, item->data(1, Qt::DisplayRole).toInt());
        break;
    }
    case nooba::DoubleParam:
    {
        p->onDoubleParamUpdate(varName, item->data(1, Qt::DisplayRole).toDouble());
        break;
    }
    case nooba::StringParam:
    {
        p->onStringParamUpdate(varName, item->text(column));
        break;
    }
    case nooba::MultiValueParam:
    {
        p->onMultiValParamUpdate(varName, item->text(column));
        break;
    }
    default:
        return;
    }
    return;
}

template<typename Map, typename Type>
void ParamConfigWind::addToTree(Map& map, Type type, QTreeWidgetItem* topLevel)
{
    typename Map::ConstIterator itr;           // typename needed for nested types
    for(itr = map.constBegin(); itr != map.constEnd(); itr++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(topLevel);
        item->setText(0, itr.key());
        item->setText(1, QString("%1").arg(itr.value()->_val));

        QVariant var = qVariantFromValue(type);
        item->setData(1, nooba::ParamTypeRole, var);
        var = qVariantFromValue(itr.value());
        item->setData(1, nooba::ParamDataRole, var);
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable | Qt::ItemIsEnabled);
    }
}
