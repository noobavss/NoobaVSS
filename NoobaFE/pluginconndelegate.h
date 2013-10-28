#ifndef PLUGINCONNDELEGATE_H
#define PLUGINCONNDELEGATE_H

#include <QStyledItemDelegate>
class PluginLoader;

class PluginConnDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PluginConnDelegate(PluginLoader* loader, QObject *parent = 0);

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private:
    PluginLoader*   _plugLoader;
};

#endif // PLUGINCONNDELEGATE_H
