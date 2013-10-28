#include "pluginconndelegate.h"
#include "PluginLoader.h"
#include "NoobaPlugin.h"

#include <QComboBox>

PluginConnDelegate::PluginConnDelegate(PluginLoader* loader, QObject *parent) :
    QStyledItemDelegate(parent),
    _plugLoader(loader)
{
}

QWidget *PluginConnDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return new QComboBox(parent);
}

void PluginConnDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox* c = qobject_cast<QComboBox* >(editor);
    if(!c)
        return;

    QList<NoobaPlugin* > data;
    if(index.column() == 0) // output plugin
    {
        data = _plugLoader->getOutputPluginList("");
    }
    else    // input plugin
    {
        data = _plugLoader->getInputPluginList("");

    }
    foreach(NoobaPlugin* p, data)
    {
        QVariant v;
        v.setValue(p);
        c->addItem(p->alias(), v);
    }
}

void PluginConnDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox* c = qobject_cast<QComboBox* >(editor);
    if(!c)
        return;

    model->setData(index, c->currentText());
    model->setData(index, c->itemData(c->currentIndex()), Qt::UserRole); // saved to model
}
