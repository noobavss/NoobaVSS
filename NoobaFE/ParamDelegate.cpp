#include "ParamDelegate.h"
#include "noobapluginapi.h"
#include "NoobaPlugin.h"
#include "NoobaEye.h"

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QVariant>

ParamDelegate::ParamDelegate(QObject* parent ):
    QStyledItemDelegate(parent)
{
}

QWidget* ParamDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    nooba::ParamType type = index.data(nooba::ParamTypeRole).value<nooba::ParamType>();
    switch(type)
    {
    case nooba::IntParam:
    {
        return new QSpinBox(parent);
    }
    case nooba::DoubleParam:
    {
        return new QDoubleSpinBox(parent);
    }
    case nooba::StringParam:
    {
        return new QLineEdit(parent);
    }
    default:
        return NULL;
    }
    return NULL;
}

void ParamDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QStyledItemDelegate::setEditorData(editor, index);
}

void ParamDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QStyledItemDelegate::setModelData(editor, model, index);
}
