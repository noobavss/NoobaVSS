#include "NoobaPlugin.h"
#include "PluginLoader.h"

#include <noobapluginapi.h>
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
    _basePlugin(NULL),
    _dir("plugins")
{
    // init plugin loader
    loadPluginInfo();
    loadPlugin(prevLoadedPluginfileName());
}

PluginLoader::~PluginLoader()
{
    if(_basePlugin)
        _basePlugin->release();

//    saveSettings(_activePluginFileName);
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
        QPluginLoader pluginLoader(_dir.absoluteFilePath(fileName)); // use a scoped pluginLoader (separate from the m_QPluginLoader ) instance so that loading
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

NoobaPlugin *PluginLoader::loadPlugin(const QString& fileName, bool isBase)
{
    // call with empty filename will occur when app is run for the first time in a 
    // system. Since this func is called by constructor of this class using previously loaded
    // plugins filename which is invalid when running for the first time
    if(fileName.isEmpty())
    {
        qDebug() << "Trying to load a plugin with an empty file name." << Q_FUNC_INFO;
        return NULL;
    }

    QPluginLoader *pl = new QPluginLoader(_dir.absoluteFilePath(fileName), this);
    QObject *plugin = pl->instance();

    if (plugin) // if plugin loading successful
    {
        NoobaPluginAPI* api = qobject_cast<NoobaPluginAPI* >(plugin);
        if (api)
        {            
            NoobaPlugin *nPlug = new NoobaPlugin(
                                    fileName,
                                    getPluginAlias(api->getPluginInfo().name()),
                                    api, pl, this);

            nPlug->setIsBasePlugin(isBase);
            nPlug->init();
            _loadedPlugins.append(nPlug);
            emit pluginLoaded(nPlug);
            if(isBase)
                _basePlugin = nPlug;
            return nPlug; // return plugin, successful
        }
    }    
    
    // on unsuccessful plugin loading
    pl->unload();       // unload
    delete pl;          // delete loader
    QMessageBox msgBox;
    msgBox.setText(tr("Plugin: %1 loading failed").arg(fileName));
    qDebug() << tr("Plugin: %1 loading failed").arg(fileName) << Q_FUNC_INFO;
    msgBox.exec();      // show error message
    return NULL;   // no plugin loaded, Unsuccessful
}

bool PluginLoader::unloadPlugins()
{
    bool ok = true;
    disconnectAllPlugins();

    foreach(NoobaPlugin* plugin, _loadedPlugins)
    {
        if(plugin) // plugin reference is not null
        {
            if(plugin->getPluginLoader())   // if plugin loader reference is not null
            {
                plugin->getPluginLoader()->unload();
                delete plugin->getPluginLoader();
            }
            else
            {
                ok = false;
            }
            emit pluginUnloaded(plugin->alias());
            delete plugin;
        }
        else
        {
            ok = false;
        }
    }
    _loadedPlugins.clear();
    _basePlugin = NULL;

    if(!ok) // on error print a debug message
    {
        qDebug() << tr("Some of the plugins are not unloaded properly.") << Q_FUNC_INFO;
    }
    return ok;
}

bool PluginLoader::unloadPlugin(const QString &alias)
{
    for(int i=0; i < _loadedPlugins.size(); i++) {
        if(_loadedPlugins.at(i)->alias().compare(alias) == 0)
        {
            bool ok = _loadedPlugins.at(i)->getPluginLoader()->unload();
            updateBasePlugin(_loadedPlugins.at(i));
            disconnectPlugin(_loadedPlugins.at(i));
            _loadedPlugins.removeAt(i);
            if(ok)
                qDebug() << tr("Plugin '%1' unloaded").arg(alias) << Q_FUNC_INFO;

            emit pluginUnloaded(alias);
            return ok;
        }
    }
    qDebug() << tr("plugin '%1' unload failed").arg(alias) << Q_FUNC_INFO;
    return false;
}

const QList<NoobaPlugin* > PluginLoader::getOutputPluginList(const QString &inPlugAlias) const
{
    if(inPlugAlias.isEmpty())
        return _loadedPlugins;

    //TODO do a check for compatibility and return list appropriately

    return _loadedPlugins;
}

const QList<NoobaPlugin* > PluginLoader::getInputPluginList(const QString &outPlugAlias) const
{

    if(outPlugAlias.isEmpty())
        return _loadedPlugins;

    // TODO do a check for compatibility and return list appropriately.
    return _loadedPlugins;
}

bool PluginLoader::connectAllPlugins(QList<PluginLoader::PluginConnData *> configList)
{
    // NOTE:    NO checking for compatibility of plugins done here. It is assumed that is done in
    //          plugin configuration.

    disconnectAllPlugins(); // disconnect all plugins and connect new plugins
    qDebug() << tr("plugin connected") << Q_FUNC_INFO;
    foreach(PluginLoader::PluginConnData* connData, configList)
    {
        connect(connData->_outPlug, SIGNAL(outputData(PluginPassData*)), connData->_inPlug, SLOT(inputData(PluginPassData*)));
    }
    _pcdList = configList;
    return true;
}

void PluginLoader::connectPlugins(PluginLoader::PluginConnData *pcd)
{
    connect(pcd->_outPlug, SIGNAL(outputData(PluginPassData*)), pcd->_inPlug, SLOT(inputData(PluginPassData*)));
    _pcdList.append(pcd);
    return;
}

bool PluginLoader::disconnectPlugin(PluginLoader::PluginConnData *pcd)
{
    bool ok = false;
    disconnect(pcd->_outPlug, SIGNAL(outputData(PluginPassData*)), pcd->_inPlug, SLOT(inputData(PluginPassData*)));
    for(int i = 0; i < _pcdList.count(); i++)
    {
        if(_pcdList.at(i) == pcd)
        {
            delete _pcdList.at(i);
            _pcdList.removeAt(i);
            ok = true;
            break;
        }
    }
    return ok;
}

bool PluginLoader::disconnectAllPlugins()
{
    foreach (PluginLoader::PluginConnData* pcd, _pcdList) {
        disconnect(pcd->_outPlug, SIGNAL(outputData(PluginPassData*)), pcd->_inPlug, SLOT(inputData(PluginPassData*)));
        delete pcd;
    }
    _pcdList.clear();
    return true;
}

bool PluginLoader::disconnectPlugin(NoobaPlugin *plugin)
{
    for(int i= _pcdList.count()-1; i >=0; i--)
    {
        PluginConnData* pcd = _pcdList.at(i);
        if(pcd->_inPlug == plugin || pcd->_outPlug == plugin)
        {
            disconnect(pcd->_outPlug, SIGNAL(outputData(PluginPassData*)), pcd->_inPlug, SLOT(inputData(PluginPassData*)));
            emit pluginsDisconnected(pcd);
            delete _pcdList.at(i);
            _pcdList.removeAt(i);
            return true;
        }
    }
    return false;
}

void PluginLoader::updateBasePlugin(NoobaPlugin* pluginToBeRemoved)
{
    if(!pluginToBeRemoved->isBasePlugin())
        return;

    _basePlugin = NULL;
    if(!_loadedPlugins.isEmpty())
        _basePlugin = _loadedPlugins.first();

    emit basePluginChanged(_basePlugin);
    return;
}

void PluginLoader::saveSettings( const QString& pluginFileName )
{
    QSettings s(nooba::Organisation, nooba::ProgramName);
    s.beginGroup(nooba::Settings_PluginInfo_block);
    s.setValue(nooba::Settings_ActivePlugin_FileName, pluginFileName);
    s.endGroup();
    s.sync();
}

// TODO:  need to load the plugin configuration
QString PluginLoader::prevLoadedPluginfileName() const
{
    QSettings s(nooba::Organisation, nooba::ProgramName);
    s.beginGroup(nooba::Settings_PluginInfo_block);
    QString filename = s.value(nooba::Settings_ActivePlugin_FileName, "").toString();
    s.endGroup();
    return filename;
}


QString PluginLoader::getPluginAlias(const QString& pluginName)
{
    int count = 1;
    foreach(NoobaPlugin* p, _loadedPlugins)
    {
        if(p->getPluginInfo().name().compare(pluginName) == 0)
            count++;
    }
    return QString("%1[%2]").arg(pluginName).arg(count);
}
