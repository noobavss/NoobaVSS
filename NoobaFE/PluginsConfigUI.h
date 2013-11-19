#ifndef PLUGINSCONFIGUI_H
#define PLUGINSCONFIGUI_H

//Qt
#include <QDialog>
#include  <QStringList>

#include "ui_PluginsConfigUI.h"
#include "pluginconndelegate.h"
#include "PluginLoader.h"

class NoobaPlugin;

class PluginsConfigUI : public QDialog
{
    Q_OBJECT

public:

    PluginsConfigUI(PluginLoader& pluginLoader, QWidget* parent = 0);
    ~PluginsConfigUI();
    QStringList getOutputPluginList(const QString& inPlugAlias);
    QStringList getInputPluginList(const QString& outPlugAlias);

private slots:

    void onReloadButtonClicked();
    void onSetDefaultButtonClicked();    
    void on_addButton_clicked();
    void on_removeButton_clicked();
    void on_applyButton_clicked();
    void on_loadPluginButton_clicked();
    void on_unloadPluginButton_clicked();
    void on_cancelButton_clicked();

    void on_doneButton_clicked();

signals:

    void loadPlugins(QStringList);
    void unloadPlugin(QString alias);
    void unloadAllPlugins();
    void loadPluginInfo();
    void saveCurrentConfig();
    void connectPlugins(QStringList outPlugList, QStringList inPugList);

private:

    void removeConnections(const QString& pluginAlias);
    void updateUI();
    void addLoadedPluginToTree(NoobaPlugin* p);
    void MoveTreeWidgetItem(QTreeWidget* from, QTreeWidget* to, QTreeWidgetItem *item);

    Ui::PluginsConfigUI ui;
    Qt::ItemDataRole    FILENAME_ROLE;
    int                 ALIAS_ROLE;
    PluginLoader        &_pluginLoader;
    PluginConnDelegate* _pluginConnDelegate;
    const QString       ACTIVE;
    const QString       DISABLED;
};

#endif // PLUGINSCONFIGUI_H
