#ifndef PLUGINSCONFIGUI_H
#define PLUGINSCONFIGUI_H

#include <QDialog>
#include "ui_PluginsConfigUI.h"
#include "pluginconndelegate.h"
#include "PluginLoader.h"

class NoobaPlugin;

class PluginsConfigUI : public QDialog
{
    Q_OBJECT

public:

    PluginsConfigUI(PluginLoader& pluginLoader, QWidget *parent = 0);
    ~PluginsConfigUI();

private slots:

    void onReloadButtonClicked();
    void onSetDefaultButtonClicked();    
    void on_addButton_clicked();
    void on_removeButton_clicked();
    void on_applyButton_clicked();
    void on_loadPluginButton_clicked();
    void on_unloadPluginButton_clicked();
    void on_cancelButton_clicked();
    void onPluginsDisconnected(PluginLoader::PluginConnData* pcd);
    void closeEvent(QCloseEvent *);

private:

    void updateUI();
    void addLoadedPluginToTree(NoobaPlugin* p);

    Ui::PluginsConfigUI ui;
    Qt::ItemDataRole    FILENAME_ROLE;
    int                 ALIAS_ROLE;
    bool                _applyConfig;
    PluginLoader        &_pluginLoader;
    PluginConnDelegate* _pluginConnDelegate;
    const QString       ACTIVE;
    const QString       DISABLED;    
};

#endif // PLUGINSCONFIGUI_H
