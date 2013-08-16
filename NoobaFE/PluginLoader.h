#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

// projects includes
#include "noobapluginapi.h"
#include "NoobaEye.h"

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

    /* Get plugin details of plugins in the plugins directory
     */
    int loadPluginInfo();

    /* Returns currently available plugins information
       NOTE: Need to call loadPlugins before this if you need to get an upto date 
       plugins information.
     */
    const QList<nooba::PluginData> getPluginInfo() const { return pluginInfoList; }

    /* Load the plugin in the plugins directory with the file name fileName
     */
    NoobaPluginAPI* loadPlugin(const QString fileName);

    NoobaPluginAPI* getActivePlugin() const { return m_activePlugin; }

    /**
     * @brief unloadActivePlugin    unloads the actvie plugin.
     * @return  if successfull returns true, otherwise false. incase there is no active
     *          plugin returns true.
     */
    bool unloadActivePlugin();

    /* Returns the plugins file name (Note: plugin file name and plugin name may be different)
     */
    QString getActivePluginFilename() const { return m_activePluginFileName; }
    

private:

    void saveSettings(const QString& pluginFileName );
    QString prevLoadedPluginfileName() const;
    	
    QList<nooba::PluginData>    pluginInfoList;
    NoobaPluginAPI             *m_activePlugin;
    QString                     m_activePluginFileName;
    QPluginLoader               m_QPluginLoader;
    QDir                        m_Dir;
};

#endif // PLUGINLOADER_H
