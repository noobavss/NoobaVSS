#include <noobapluginapi.h>

#include "PluginLoader.h"

// Qt includes 
#include <QDir>
#include <QPluginLoader>
#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QObject>
#include <QSettings>

PluginLoader::PluginLoader(QObject *parent)
	: QObject(parent),
    _activePlugin(NULL),
    _dir(QApplication::instance()->applicationDirPath())
{
    _dir.cd("plugins");    // look in plugins directory
    // init plugin loader
    loadPluginInfo();
    loadPlugin(prevLoadedPluginfileName());
}

PluginLoader::~PluginLoader()
{
    if(_activePlugin)
        _activePlugin->release();

    saveSettings(_activePluginFileName);
}


int PluginLoader::loadPluginInfo()
{
    /*QDir pluginsDir(QApplication::instance()->applicationDirPath());*/
    _pluginInfoList.clear();
    int pluginCount = 0;

    int pluginAPIMajorVer;
    int pluginAPIMinorVer;
    QString errStr;

    // go through the directory and try to load the plugin files
    foreach (QString fileName, _dir.entryList(QDir::Files))
    {        
        QPluginLoader pluginLoader(_dir.absoluteFilePath(fileName));       // use a scoped pluginLoader (separate from the m_QPluginLoader ) instance so that loading
                                                                            // plugins to retrieve information and unloading doesn't affect the currently loaded plugin
        QObject *plugin = pluginLoader.instance();
        if (!plugin)
            break;

        NoobaPluginAPIBase* apiBase = qobject_cast<NoobaPluginAPIBase *>(plugin);
        if (!apiBase)
            break;

        pluginAPIMajorVer = apiBase->APIMajorVersion();
        pluginAPIMinorVer = apiBase->APIMinorVersion();
        bool ok = true;
        if(pluginAPIMajorVer > API_MAJOR_VERSION)
            ok = false;

        if(pluginAPIMajorVer == API_MAJOR_VERSION && pluginAPIMinorVer > API_MINOR_VERSION)
            ok = false;

        if(!ok)
        {
            errStr.append( tr("\n%1 version(%2.%3) is newer than the program version (%4.%5). Plugin not loaded ").
                  arg(fileName).arg(pluginAPIMajorVer).arg(pluginAPIMinorVer).arg(API_MAJOR_VERSION).arg(API_MINOR_VERSION));
            break;
        }

        // add the plugin details to the list
        NoobaPluginAPI* api = qobject_cast<NoobaPluginAPI* >(plugin);
        nooba::PluginData pluginData(fileName, api->getPluginInfo(),pluginAPIMajorVer, pluginAPIMinorVer);
        _pluginInfoList.append(pluginData);
        pluginLoader.unload();  // unload after getting the details of the plugin
        pluginCount++;

        pluginLoader.unload();   // additional unload call, just to be safe :)
    }

    if(!errStr.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Some plugins were not loaded."));
        msgBox.setDetailedText(tr("%1").arg(errStr));
        msgBox.exec();
    }
    return pluginCount;
}

NoobaPlugin *PluginLoader::loadPlugin( const QString fileName )
{
    unloadActivePlugin();
    // call with empty filename will occur when app is run for the first time in a 
    // system. Since this func is called by constructor of this class using previously loaded
    // plugins filename which is invalid when running for the first time
    if(fileName.isEmpty())
    {
        qDebug() << "empty plugin file name" << Q_FUNC_INFO; 
        return NULL;
    }


    _QPluginLoader.setFileName(_dir.absoluteFilePath(fileName));
    QObject *plugin = _QPluginLoader.instance();

    if (plugin)
    {
        NoobaPluginAPI* api = qobject_cast<NoobaPluginAPI* >(plugin);
        if (api)
        {
            _activePlugin = new NoobaPlugin(api, this);
            _activePluginFileName = fileName;
            _activePlugin->init();
            emit pluginLoaded(_activePlugin);
            return _activePlugin; // return plugin, successful
        }
    }    
    
    QMessageBox msgBox;
    msgBox.setText(tr("Plugin: %1 loading failed").arg(fileName));
    qDebug() << tr("Plugin: %1 loading failed").arg(fileName) << Q_FUNC_INFO;
    msgBox.exec();
    return NULL;   // no plugin loaded, Unsuccessful
}

bool PluginLoader::unloadActivePlugin()
{
    if(_activePlugin)
    {
        delete _activePlugin;
        _activePlugin = NULL;
    }

    _QPluginLoader.unload();
    _activePluginFileName.clear();
    emit pluginUnloaded();
    return true;
}

void PluginLoader::saveSettings( const QString& pluginFileName )
{
    QSettings s(nooba::Organisation, nooba::ProgramName);
    s.beginGroup(nooba::Settings_PluginInfo_block);
    s.setValue(nooba::Settings_ActivePlugin_FileName, pluginFileName);
    s.endGroup();
    s.sync();
}

QString PluginLoader::prevLoadedPluginfileName() const
{
    QSettings s(nooba::Organisation, nooba::ProgramName);
    s.beginGroup(nooba::Settings_PluginInfo_block);
    QString filename = s.value(nooba::Settings_ActivePlugin_FileName, "").toString();
    s.endGroup();
    return filename;
}
