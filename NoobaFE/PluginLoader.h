#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

// projects includes
#include "NoobaEye.h"

// Qt includes
#include <QObject>
#include <QList>
#include <QPluginLoader>
#include <QDir>
 
class NoobaPlugin;

class PluginLoader : public QObject
{
	Q_OBJECT

public:

    struct PluginConnData {

        QString         _outPlugAlias;
        QString         _inPlugAlias;
        NoobaPlugin*    _inPlug;
        NoobaPlugin*    _outPlug;
    };

    PluginLoader(QObject *parent = 0);
	~PluginLoader();

    /**
     * Get plugin details of plugins in the plugins directory
     */
    int loadPluginInfo();

    /**
     * Returns currently available plugins information
     * NOTE: Need to call loadPlugins before this if you need to get an upto date
     * plugins information.
     */
    const QList<nooba::PluginData> getPluginInfo() const { return _pluginInfoList; }

    /**
     * @brief loadPlugin Load the plugin in the plugins directory with the file name fileName
     * @param fileName
     * @param isBase
     * @return
     */
    NoobaPlugin* loadPlugin(const QString& fileName, bool isBase = false);

    /**
     * @brief getActivePlugin main plugin that does the initial processing. Only this plugins ProcFrame(..)
     *                        functions is called by the FE.
     * @return
     */
    NoobaPlugin* getBasePlugin() const { return _basePlugin; }

    /**
     * @brief getActivePlugins currently loaded plugins are returned
     * @return plugins are returned as a QList od NoobaPlugin pointers.
     */
    QList<NoobaPlugin* > getActivePlugins() const { return _loadedPlugins; }
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
    bool unloadPlugin(const QString& alias);

    const QList<NoobaPlugin* > getOutputPluginList(const QString& inPlugAlias) const;
    const QList<NoobaPlugin* > getInputPluginList(const QString& outPlugAlias) const;

    /**
     * @brief connectPlugins    All previous connections are disconnected before connecting the
     *                          given set of plugins.
     * @param configList
     * @return
     */
    bool connectAllPlugins(QList<PluginConnData*> configList);
    void connectPlugins(PluginConnData *pcd);

    /**
     * @brief getPCDList returns the PluginConnData list
     * @return
     */
    QList<PluginConnData*> getPCDList() { return _pcdList; }

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
    void reloadPlugins();

signals:

    void pluginLoaded(NoobaPlugin *plugin);
    void pluginUnloaded(const QString& alias);
    void pluginsDisconnected(PluginLoader::PluginConnData* pcd);
    void pluginsConnected(PluginLoader::PluginConnData* pcd);
    void pluginInitialised(NoobaPlugin* plugin);
    void pluginAboutToRelease(NoobaPlugin* plugin);

    /**
     * @brief basePluginChanged
     * @param newBasePlugin     Null if there's no base plugin
     */
    void basePluginChanged(NoobaPlugin* newBasePlugin);

private:

    inline bool versionCheckOk(NoobaPluginAPIBase* apiBase, const QString &filename, QString &errStr);
    inline QString getPluginAlias(const QString &pluginName);
    inline void updateConnection(PluginConnData* pcd, bool isConnect);
    void updateBasePlugin(NoobaPlugin *pluginToBeRemoved);

    QList<nooba::PluginData>    _pluginInfoList;
    NoobaPlugin                 *_basePlugin;
    QDir                        _dir;
    QList<PluginConnData*>      _pcdList;
    QList<NoobaPlugin* >        _loadedPlugins;
};

#endif // PLUGINLOADER_H
