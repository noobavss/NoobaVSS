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
