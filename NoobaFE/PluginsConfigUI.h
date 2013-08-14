#ifndef PLUGINSCONFIGUI_H
#define PLUGINSCONFIGUI_H

#include <QDialog>
#include "ui_PluginsConfigUI.h"

class PluginLoader;

class PluginsConfigUI : public QDialog
{
    Q_OBJECT

public:

    PluginsConfigUI(PluginLoader& pluginLoader, QWidget *parent = 0);
    ~PluginsConfigUI();

private slots:

    void onReloadButtonClicked();
    void onSetDefaultButtonClicked();
    
private:

    void updateUI();

    Ui::PluginsConfigUI ui;
    PluginLoader        &m_pluginLoader;
    const QString       ACTIVE;
    const QString       DISABLED;    

};

#endif // PLUGINSCONFIGUI_H
