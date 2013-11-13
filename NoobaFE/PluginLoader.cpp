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
}

PluginLoader::~PluginLoader()
{
    saveCurrentConfig();    // this should be at the TOP
    if(_basePlugin)
        _basePlugin->release();

    unloadPlugins();
}

int PluginLoader::loadPluginInfo()
{
    /*QDir pluginsDir(QApplication::instance()->applicationDirPath());*/
    _pluginInfoList.clear();
    int pluginCount = 0;
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

        if(!versionCheckOk(apiBase, fileName,  errStr))
            break;

        // add the plugin details to the list
        NoobaPluginAPI* api = qobject_cast<NoobaPluginAPI* >(plugin);
        nooba::PluginData pluginData(fileName, api->getPluginInfo(),api->APIMajorVersion(), api->APIMinorVersion());
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
            emit pluginLoaded(nPlug); // this should be called imediately after creation of NoobaPlugin

            connect(nPlug, SIGNAL(onInit(NoobaPlugin*)), this, SIGNAL(pluginInitialised(NoobaPlugin*)));
            connect(nPlug, SIGNAL(onAboutToRelease(NoobaPlugin*)), this, SIGNAL(pluginAboutToRelease(NoobaPlugin*)));

            nPlug->setIsBasePlugin(isBase);
            nPlug->init();
            _loadedPlugins.append(nPlug);
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
    return NULL;        // no plugin loaded, Unsuccessful
}

bool PluginLoader::unloadPlugins()
{
    bool ok = true;
    disconnectAllPlugins();
    _basePlugin = NULL;

    foreach(NoobaPlugin* plugin, _loadedPlugins)
    {
        if(plugin) // plugin reference is not null
        {
            releaseAndUnload(plugin);
        }
        else
        {
            ok = false;
        }
    }
    _loadedPlugins.clear();

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
            NoobaPlugin* p = _loadedPlugins.at(i);
            updateBasePlugin(p);
            disconnectPlugin(p);
            releaseAndUnload(p);
            _loadedPlugins.removeAt(i);
            return true;
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

    disconnectAllPlugins();         // disconnect all plugins and connect new plugins
    qDebug() << tr("plugin connected") << Q_FUNC_INFO;
    foreach(PluginLoader::PluginConnData* pcd, configList)
    {
        updatePluginConnection(pcd, true);
    }
    _pcdList = configList;
    return true;
}

void PluginLoader::connectPlugins(PluginLoader::PluginConnData* pcd)
{
    updatePluginConnection(pcd, true);
    _pcdList.append(pcd);
    return;
}

bool PluginLoader::disconnectPlugin(PluginLoader::PluginConnData *pcd)
{
    bool ok = false;
    updatePluginConnection(pcd, false);
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
        updatePluginConnection(pcd, false);
        delete pcd;
    }
    _pcdList.clear();
    return true;
}

void PluginLoader::saveCurrentConfig()
{
    QSettings s(nooba::Organisation, nooba::ProgramName);

    s.beginGroup(nooba::Settings_PluginConfig_block);
    s.remove("");
    if(_basePlugin)
    {
        s.setValue(nooba::Settings_BasePlugin_Filename, _basePlugin->fileName());

        QStringList activePlugins;
        foreach(NoobaPlugin* p, _loadedPlugins)
        {
            activePlugins.append(p->fileName());
        }

        s.setValue(nooba::Settings_ActivePluginList, QVariant(activePlugins));

        // plugin connection data
        s.beginWriteArray(nooba::Settings_Connection_data);
        for(int i = 0; i < _pcdList.count(); i++)
        {
            s.setArrayIndex(i);
            s.setValue(nooba::Settings_InPlug_Alias, _pcdList.at(i)->_inPlugAlias);
            s.setValue(nooba::Settings_OutPlug_Alias, _pcdList.at(i)->_outPlugAlias);
        }
        s.endArray();
    }
    s.endGroup();
    s.sync();
}

void PluginLoader::loadPrevConfig()
{
    QSettings s(nooba::Organisation, nooba::ProgramName);
    s.beginGroup(nooba::Settings_PluginConfig_block);
    QString baseFilename = s.value(nooba::Settings_BasePlugin_Filename, "").toString();
    QStringList activePlugins = s.value(nooba::Settings_ActivePluginList, "").toStringList();

    unloadPlugins();        // unload previously loaded plugins

    // load plugins
    bool isBase;
    foreach(const QString& fName, activePlugins)
    {
        if(fName.isEmpty()) // don't try to load, ignore
            continue;
        (fName.compare(baseFilename) == 0) ? isBase = true: isBase = false;
        NoobaPlugin* p = loadPlugin(fName, isBase);
        if(!p)
        {
            unloadPlugins();
            QMessageBox msgBox;
            msgBox.setText(tr("Plugin loading from previous configuration failed. %1 not found").arg(fName));
            msgBox.exec();
            return;
        }
    }

    // make connections
    int size = s.beginReadArray(nooba::Settings_Connection_data);
    for(int i = 0; i < size; i++)
    {
        // load each connection detail from QSettings
        s.setArrayIndex(i);
        PluginConnData* pcd = new PluginConnData;
        pcd->_inPlugAlias = s.value(nooba::Settings_InPlug_Alias, tr("!Error")).toString();
        pcd->_outPlugAlias = s.value(nooba::Settings_OutPlug_Alias, tr("!Error")).toString();
        bool inFound = false, outFound = false;
        foreach(NoobaPlugin* p, _loadedPlugins) // search for the loaded plugins for the relevant
                                                // plugins to be connected.
        {
            if(p->alias().compare(pcd->_inPlugAlias) == 0)
            {
                pcd->_inPlug = p;
                inFound = true;
            }
            if(p->alias().compare(pcd->_outPlugAlias) == 0)
            {
                pcd->_outPlug = p;
                outFound = true;
            }
            if(outFound && inFound)
                break;
        }
        if(!inFound || !outFound) // if one of the plugins not found remove connection
        {
            delete pcd;
        }
        connectPlugins(pcd);    // connect the plugins
    }
    s.endArray();

    s.endGroup();
    return;
}

void PluginLoader::refreshPlugins()
{
    foreach (NoobaPlugin* p, _loadedPlugins) {
        p->release();
        p->init();        
    }
}

bool PluginLoader::versionCheckOk(NoobaPluginAPIBase *apiBase, const QString& filename, QString& errStr)
{
    int pluginAPIMajorVer = apiBase->APIMajorVersion();
    int pluginAPIMinorVer = apiBase->APIMinorVersion();
    bool ok = true;
    if(pluginAPIMajorVer > API_MAJOR_VERSION)
        ok = false;

    if(pluginAPIMajorVer == API_MAJOR_VERSION && pluginAPIMinorVer > API_MINOR_VERSION)
        ok = false;

    if(pluginAPIMajorVer == 0 && pluginAPIMinorVer < 5) // api compatibility brocked in 0.4 to 0.5
    {
        errStr.append(tr("\n%1 version(%2.%3) is incompatible with the current version (%4.%5). Recompile the plugin" \
                         " with the latest version of the api.").arg(filename).arg(pluginAPIMajorVer).
                         arg(pluginAPIMinorVer).arg(API_MAJOR_VERSION).arg(API_MINOR_VERSION));
        return false;
    }

    if(!ok)
    {
        errStr.append( tr("\n[version mismatch] %1 version(%2.%3) is newer than the program version (%4.%5).").
              arg(filename).arg(pluginAPIMajorVer).arg(pluginAPIMinorVer).arg(API_MAJOR_VERSION).arg(API_MINOR_VERSION));
        return false;
    }
    return true;
}

bool PluginLoader::disconnectPlugin(NoobaPlugin *plugin)
{
    for(int i= _pcdList.count()-1; i >=0; i--)
    {
        PluginConnData* pcd = _pcdList.at(i);
        if(pcd->_inPlug == plugin || pcd->_outPlug == plugin)
        {
            updatePluginConnection(pcd, false);
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
    {
        foreach(NoobaPlugin* p, _loadedPlugins)
        {
            if(p != pluginToBeRemoved)
            {
                _basePlugin = p;
                break;
            }
        }
    }
    emit basePluginChanged(_basePlugin);
    return;
}

void PluginLoader::releaseAndUnload(NoobaPlugin *plugin)
{
    // release then unload
    plugin->release();
    emit pluginAboutToUnloaded(plugin);  // this should be called after release();
    delete plugin;
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

void PluginLoader::updatePluginConnection(PluginLoader::PluginConnData *pcd, bool isConnect)
{
    if(isConnect)
    {
        connect(pcd->_outPlug, SIGNAL(outputData(PluginPassData)), pcd->_inPlug, SLOT(inputData(PluginPassData)));
        emit pluginsConnected(pcd);
    }
    else
    {
        disconnect(pcd->_outPlug, SIGNAL(outputData(PluginPassData)), pcd->_inPlug, SLOT(inputData(PluginPassData)));
        emit pluginsDisconnected(pcd);
    }
    return;
}
