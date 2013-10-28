#ifndef PARAMDELEGATE_H
#define PARAMDELEGATE_H

#include <QStyledItemDelegate>

class NoobaPlugin;

/**
 * @brief The ParamDelegate class
 */
class ParamDelegate : public QStyledItemDelegate
{
public:

    ParamDelegate(QObject* parent = 0);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};

/**
 * @brief The NoEditDelegate class
 */
class NoEditDelegate: public QStyledItemDelegate
{
public:

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
    {
        Q_UNUSED(parent)
        Q_UNUSED(option)
        Q_UNUSED(index)
        return NULL;
    }
};

#endif // PARAMDELEGATE_H
