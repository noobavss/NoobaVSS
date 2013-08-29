#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

// projects includes
#include "NoobaEye.h"
#include "NoobaPlugin.h"

// Qt includes
#include <QObject>
#include <QList>
#include <QPluginLoader>
#include <QDir>
 
class PluginLoader : public QObject
{
	Q_OBJECT

public:

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
     * Load the plugin in the plugins directory with the file name fileName
     */
    NoobaPlugin* loadPlugin(const QString fileName);

    NoobaPlugin* getActivePlugin() const { return _activePlugin; }

    /**
     * @brief unloadActivePlugin    unloads the actvie plugin.
     * @return  if successfull returns true, otherwise false. incase there is no active
     *          plugin returns true.
     */
    bool unloadActivePlugin();

    /* Returns the plugins file name (Note: plugin file name and plugin name may be different)
     */
    QString getActivePluginFilename() const { return _activePluginFileName; }

signals:

    void pluginLoaded(NoobaPlugin *plugin);
    void pluginUnloaded();

private:

    void saveSettings(const QString& pluginFileName );
    QString prevLoadedPluginfileName() const;
    	
    QList<nooba::PluginData>    _pluginInfoList;
    NoobaPlugin                 *_activePlugin;
    QString                     _activePluginFileName;
    QPluginLoader               _QPluginLoader;
    QDir                        _dir;
};

#endif // PLUGINLOADER_H
