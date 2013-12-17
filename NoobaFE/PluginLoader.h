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

#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

// projects includes
#include "NoobaEye.h"

// Qt includes
#include <QObject>
#include <QList>
#include <QPluginLoader>
#include <QDir>
#include <QMutex>
 
class NoobaPlugin;

struct PluginConnData {

    QString         _outPlugAlias;
    QString         _inPlugAlias;
    NoobaPlugin*    _inPlug;
    NoobaPlugin*    _outPlug;
};

Q_DECLARE_METATYPE(PluginConnData)
Q_DECLARE_METATYPE(PluginConnData*)

class PluginLoader : public QObject
{
	Q_OBJECT

public:

    PluginLoader(QObject *parent = 0);
	~PluginLoader();

    /**
     * Returns currently available plugins information
     * NOTE: Need to call loadPlugins before this if you need to get an upto date
     * plugins information.
     */
    const QList<nooba::PluginData> getPluginInfo() const { return _pluginInfoList; }

    /**
     * @brief getActivePlugin main plugin that does the initial processing. Only this plugins ProcFrame(..)
     *                        functions is called by the FE.
     * @return
     */
    NoobaPlugin *getBasePlugin() const;

    /**
     * @brief getActivePlugins currently loaded plugins are returned
     * @return plugins are returned as a QList od NoobaPlugin pointers.
     */
    QList<NoobaPlugin *> getActivePlugins() const;
    const QList<NoobaPlugin* > getOutputPluginList(const QString& inPlugAlias) const;
    const QList<NoobaPlugin* > getInputPluginList(const QString& outPlugAlias) const;

    /**
     * @brief getPCDList returns the PluginConnData list
     * @return
     */
    QList<PluginConnData *> getPCDList();

public slots:

    /**
     * Get plugin details of plugins in the plugins directory
     */
    int loadPluginInfo();

    void loadPlugins(QStringList filenameList);

    /**
     * @brief loadPlugin Load the plugin in the plugins directory with the file name fileName
     * @param fileName
     * @param isBase
     * @return
     */
    NoobaPlugin* loadPlugin(QString fileName, bool isBase = false);

    /**
     * @brief unloadActivePlugin    unloads the actvie plugin.
     * @return  if successfull returns true, otherwise false. incase there is no active
     *          plugin returns true.
     */
    bool unloadPlugins();

    /**
     * @brief unloadPlugin  unload the plugin with the given alias. Alias is the name given to plugin
     *                      at loading time. Same plugin can be loaded with different aliases.
     * @param alias         alias of the plugin.
     * @return              return true if the unloading was successful.
     */
    bool unloadPlugin(QString alias);

    /**
     * @brief connectPlugins    All previous connections are disconnected before connecting the
     *                          given set of plugins.
     * @param configList
     * @return
     */
    bool connectAllPlugins(QList<PluginConnData*> configList);
    void connectPlugins(PluginConnData *pcd);

    void connectPlugins(QStringList outPlugList, QStringList inPlugList);
    void connectPlugins(QString outPlugAlias, QString inPlugAlias);

    /**
     * @brief disconnectPlugin  disconnect a single connection according to the plugin connection
     *                          data provided.
     * @param pcd
     * @return
     */
    bool disconnectPlugin(PluginConnData* pcd);

    /**
     * @brief disconnectPlugin Disconnect all connections related to the given plugin
     * @param plugin
     * @return
     */
    bool disconnectPlugin(NoobaPlugin* plugin);
    bool disconnectAllPlugins();

    void saveCurrentConfig();
    void loadPrevConfig();
    void refreshPlugins();

signals:

    void pluginLoaded(NoobaPlugin* plugin);
    void pluginAboutToUnloaded(QString alias);
    void pluginsDisconnected(PluginConnData* pcd);
    void pluginsConnected(PluginConnData* pcd);
    void pluginInitialised(NoobaPlugin* plugin);
    void pluginAboutToRelease(QString alias);
    void errMsg(const QString& errMsg, const QString& detailedErrMsg = QString());

    /**
     * @brief basePluginChanged
     * @param newBasePlugin     Null if there's no base plugin
     */
    void basePluginChanged(NoobaPlugin* newBasePlugin);

private:

    inline bool versionCheckOk(NoobaPluginAPI *api, const QString &filename, QString &errStr);
    inline QString getPluginAlias(const QString &pluginName);
    inline void updatePluginConnection(PluginConnData* pcd, bool isConnect);
    void updateBasePlugin(NoobaPlugin *pluginToBeRemoved);
    inline void releaseAndUnload(NoobaPlugin* plugin);

    mutable QMutex              _basePluginMutex;
    mutable QMutex              _pcdMutex;
    mutable QMutex              _loadedPluginsMutex;
    mutable QMutex              _pluginInfoListMutex;
    QList<nooba::PluginData>    _pluginInfoList;
    NoobaPlugin                 *_basePlugin;
    QDir                        _dir;
    QList<PluginConnData*>      _pcdList;
    QList<NoobaPlugin* >        _loadedPlugins;
};

#endif // PLUGINLOADER_H
