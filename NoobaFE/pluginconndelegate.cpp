#include "pluginconndelegate.h"
#include "PluginLoader.h"
#include "NoobaPlugin.h"
#include "PluginsConfigUI.h"

#include <QComboBox>
#include <QStringList>

PluginConnDelegate::PluginConnDelegate(PluginsConfigUI* configUi) :
    QStyledItemDelegate(configUi),
    _plugConfUI(configUi)
{
}

QWidget *PluginConnDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    QComboBox* editor = new QComboBox(parent);
    QStringList data;
    if(index.column() == 0) // output plugin
    {
        data = _plugConfUI->getOutputPluginList("");
    }
    else    // input plugin
    {
        data = _plugConfUI->getInputPluginList("");

    }

    foreach(const QString& name, data)
    {
        editor->addItem(name);
    }
    return editor;
}

void PluginConnDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QStyledItemDelegate::setEditorData(editor, index);
}

void PluginConnDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox* c = qobject_cast<QComboBox* >(editor);
    if(!c)
    {
        QStyledItemDelegate::setModelData(editor, model, index);
        return;
    }
    model->setData(index, c->currentText(), Qt::EditRole);
}
