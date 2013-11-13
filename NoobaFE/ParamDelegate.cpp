#include "ParamDelegate.h"
#include "noobapluginapi.h"
#include "NoobaPlugin.h"
#include "NoobaEye.h"

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QVariant>

ParamDelegate::ParamDelegate(QObject* parent ):
    QStyledItemDelegate(parent)
{
}

QWidget* ParamDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    nooba::ParamType type = index.data(nooba::ParamTypeRole).value<nooba::ParamType>();

    switch(type)
    {
    case nooba::IntParam:
    {
        QSpinBox* e = new QSpinBox(parent);
        IntData* d = index.data(nooba::ParamDataRole).value<IntData*>();
        e->setMaximum(d->_max);
        e->setMinimum(d->_min);
        return e;
    }
    case nooba::DoubleParam:
    {
        QDoubleSpinBox *e = new QDoubleSpinBox(parent);
        DoubleData* d = index.data(nooba::ParamDataRole).value<DoubleData*>();
        e->setMaximum(d->_max);
        e->setMinimum(d->_min);
        return e;
    }
    case nooba::StringParam:
    {
        return new QLineEdit(parent);
    }
    case nooba::MultiValueParam:
    {
        StringListData* d = index.data(nooba::ParamDataRole).value<StringListData*>();
        if(!d)
            return NULL;

        QComboBox* editor = new QComboBox(parent);
        editor->addItems(d->_varList);
        return editor;
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
