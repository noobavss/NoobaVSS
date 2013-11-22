#include "ParamDelegate.h"
#include "noobapluginapi.h"
#include "NoobaPlugin.h"
#include "NoobaEye.h"
#include "PathLineEdit.h"

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
        StringData* strData = index.data(nooba::ParamDataRole).value<StringData* >();
        if(strData->_isFilePath)
        {
            PathLineEdit* e = new PathLineEdit(parent);     // file browser edit
            e->setText(strData->_val);
            return e;
        }
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
    case nooba::FilePathParam:
    {
        FilePathData* d = index.data(nooba::ParamDataRole).value<FilePathData* >();
        if(!d)
            return NULL;

        PathLineEdit* editor = new PathLineEdit(parent);
        editor->setText(d->_val);
        editor->setFilter(d->_filter);
        editor->setFileMode(d->_pathType);
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
    nooba::ParamType type = index.data(nooba::ParamTypeRole).value<nooba::ParamType>();

    switch(type)
    {
    case nooba::StringParam:
    {
        StringData* strData = index.data(nooba::ParamDataRole).value<StringData* >();
        if(!strData)
        {
            break;
        }
        if(strData->_isFilePath)
        {
            PathLineEdit* e = qobject_cast<PathLineEdit* >(editor);
            if(!e)
                QStyledItemDelegate::setModelData(editor, model, index);

            model->setData(index, e->getText());
            break;
        }
    }
    case nooba::FilePathParam:
    {
        PathLineEdit* fpd = qobject_cast<PathLineEdit* >(editor);
        if(!fpd)
        {
            break;
        }
        PathLineEdit* e = qobject_cast<PathLineEdit* >(editor);
        if(!e)
        {
            QStyledItemDelegate::setModelData(editor, model, index);
            break;
        }
        model->setData(index, e->getText());

    }
    default:
        QStyledItemDelegate::setModelData(editor, model, index);
    }


}
