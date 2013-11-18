#ifndef PLUGINCONNDELEGATE_H
#define PLUGINCONNDELEGATE_H

#include <QStyledItemDelegate>
class PluginLoader;
class PluginsConfigUI;

class PluginConnDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PluginConnDelegate(PluginsConfigUI *configUi);

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private:
    PluginsConfigUI*   _plugConfUI;
};

#endif // PLUGINCONNDELEGATE_H
