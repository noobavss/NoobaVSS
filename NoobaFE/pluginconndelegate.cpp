/**
    @author  Asitha Nanayakkara <daun07@gmail.com>

    @section LICENSE
    Nooba Plugin API source file
    Copyright (C) 2013 Developed by Team Nooba

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
