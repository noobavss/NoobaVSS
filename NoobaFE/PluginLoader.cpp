#include "NoobaPlugin.h"
#include "PluginLoader.h"

#include <noobapluginapi.h>
// Qt includes 
#include <QDir>
#include <QPluginLoader>
#include <QApplication>
#include <QDebug>
#include <QObject>
#include <QSettings>
#include <QThread>
#include <QMutexLocker>

PluginLoader::PluginLoader(QObject *parent)
	: QObject(parent),
    _basePlugin(NULL),
    _dir(QApplication::applicationDirPath())
{
    _dir.cd("plugins");
    // init plugin loader
    loadPluginInfo();
}

PluginLoader::~PluginLoader()
{
    saveCurrentConfig();    // this should be at the TOP
    unloadPlugins();
}

NoobaPlugin *PluginLoader::getBasePlugin() const
{
    const QMutexLocker locker(&_basePluginMutex);
    return _basePlugin;
}

QList<NoobaPlugin* > PluginLoader::getActivePlugins() const
{
    QMutexLocker locker(&_loadedPluginsMutex);
    return _loadedPlugins;
}

int PluginLoader::loadPluginInfo()
{
    QMutexLocker locker(&_pluginInfoListMutex);
    /*QDir pluginsDir(QApplication::instance()->applicationDirPath());*/
    _pluginInfoList.clear();
    int pluginCount = 0;
    QString errStr;

    // go through the directory and try to load the plugin files
    foreach (QString fileName, _dir.entryList(QDir::Files))
    {        
        if(!QLibrary::isLibrary(fileName))
            continue;

        QPluginLoader qPluginLoader(_dir.absoluteFilePath(fileName)); // use a scoped pluginLoader (separate from the m_QPluginLoader ) instance so that loading
                                                                     // plugins to retrieve information and unloading doesn't affect the currently loaded plugin
        QObject *plugin = qPluginLoader.instance();
        if (!plugin)
            continue;

        NoobaPluginAPIBase* apiBase = qobject_cast<NoobaPluginAPIBase *>(plugin);
        if (!apiBase)
            continue;

        if(!versionCheckOk(apiBase, fileName,  errStr))
            continue;

        // add the plugin details to the list
        NoobaPluginAPI* api = qobject_cast<NoobaPluginAPI* >(plugin);
        nooba::PluginData pluginData(fileName, api->getPluginInfo(),api->APIMajorVersion(), api->APIMinorVersion());
        _pluginInfoList.append(pluginData);
        qPluginLoader.unload();  // unload after getting the details of the plugin
        pluginCount++;

        qPluginLoader.unload();   // additional unload call, just to be safe :)
    }

    if(!errStr.isEmpty())
    {
        emit errMsg(tr("Some plugins were not loaded."), tr("%1").arg(errStr) );
        qDebug() << errStr << Q_FUNC_INFO;
    }
    return pluginCount;
}

void PluginLoader::loadPlugins(QStringList filenameList)
{
    if(filenameList.isEmpty())
    {
        qDebug() << tr("filename list is empty. Plugin loading failed") << Q_FUNC_INFO;
        return;
    }
    loadPlugin(filenameList.first(), true);

    for(int i = 1; i < filenameList.count(); i++)
    {
        loadPlugin(filenameList.at(i));
    }
}

NoobaPlugin *PluginLoader::loadPlugin(QString fileName, bool isBase)
{
    if(fileName.isEmpty())
    {
        qDebug() << "Trying to load a plugin with an empty file name." << Q_FUNC_INFO;
        return NULL;
    }

    QPluginLoader *pl = new QPluginLoader(_dir.absoluteFilePath(fileName));
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
            connect(nPlug, SIGNAL(onAboutToRelease(QString)), this, SIGNAL(pluginAboutToRelease(QString)));

            nPlug->setIsBasePlugin(isBase);

            ///////////////////////////////////////
            // LOCK loaded plugin list
            ///////////////////////////////////////
            _loadedPluginsMutex.lock();
            _loadedPlugins.append(nPlug);
            _loadedPluginsMutex.unlock();
            ////////////////////////////////////////
            // UNLOCK
            ////////////////////////////////////////
            if(isBase)
            {
                ////////////////////////////////////
                // base plugin update lock
                ////////////////////////////////////
                _basePluginMutex.lock();
                _basePlugin = nPlug;
                _basePluginMutex.unlock();
                ////////////////////////////////////
                // UNLOCK
                ////////////////////////////////////
            }
            nPlug->init();
            return nPlug; // return plugin, successful
        }
    }    
    
    // on unsuccessful plugin loading
    pl->unload();       // unload
    delete pl;          // delete loader
    emit errMsg(tr("Plugin: %1 loading failed").arg(fileName));
    qDebug() << tr("Plugin: %1 loading failed").arg(fileName) << Q_FUNC_INFO;
    return NULL;        // no plugin loaded, Unsuccessful
}

bool PluginLoader::unloadPlugins()
{
    bool ok = true;
    disconnectAllPlugins();

    _basePluginMutex.lock();
    _basePlugin = NULL;
    _basePluginMutex.unlock();

    QMutexLocker locker(&_loadedPluginsMutex);
    if(_loadedPlugins.isEmpty())
        return ok;

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
    else {
        qDebug() << tr("All the plugins successfully unloaded") << Q_FUNC_INFO;
    }
    return ok;
}

bool PluginLoader::unloadPlugin(QString alias)
{
    qDebug() << Q_FUNC_INFO;
    QMutexLocker locker(&_loadedPluginsMutex);

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
    QMutexLocker locker(&_loadedPluginsMutex);
    if(inPlugAlias.isEmpty())
        return _loadedPlugins;

    //TODO do a check for compatibility and return list appropriately

    return _loadedPlugins;
}

const QList<NoobaPlugin* > PluginLoader::getInputPluginList(const QString &outPlugAlias) const
{
    QMutexLocker locker(&_loadedPluginsMutex);
    if(outPlugAlias.isEmpty())
        return _loadedPlugins;

    // TODO do a check for compatibility and return list appropriately.
    return _loadedPlugins;
}

QList<PluginConnData *> PluginLoader::getPCDList()
{
    QMutexLocker locker(&_pcdMutex);
    return _pcdList;
}

bool PluginLoader::connectAllPlugins(QList<PluginConnData *> configList)
{
    // NOTE:    NO checking for compatibility of plugins done here. It is assumed that is done in
    //          plugin configuration.

    disconnectAllPlugins();         // disconnect all plugins and connect new plugins
    qDebug() << tr("plugin connected") << Q_FUNC_INFO;
    foreach(PluginConnData* pcd, configList)
    {
        updatePluginConnection(pcd, true);
    }
    _pcdMutex.lock();
    _pcdList = configList;
    _pcdMutex.unlock();
    return true;
}

void PluginLoader::connectPlugins(PluginConnData* pcd)
{
    updatePluginConnection(pcd, true);
    _pcdMutex.lock();
    _pcdList.append(pcd);
    _pcdMutex.unlock();
    return;
}

void PluginLoader::connectPlugins(QStringList outPlugList, QStringList inPlugList)
{
    qDebug() << Q_FUNC_INFO;
    if(outPlugList.size() != inPlugList.size())
    {
        qDebug() << tr("intput and output plugin list size mismatch") << Q_FUNC_INFO;
        return;
    }


    for(int i = 0; i < outPlugList.size(); i++)
    {
        connectPlugins(outPlugList.at(i), inPlugList.at(i));
    }

    return;
}

void PluginLoader::connectPlugins(QString outPlugAlias, QString inPlugAlias)
{
    bool inFound = false, outFound = false;

    PluginConnData* pcd = new PluginConnData;
    QMutexLocker locker(&_loadedPluginsMutex);

    foreach (NoobaPlugin* p, _loadedPlugins)
    {
        // inPlug and outPlug both can be the same plugin
        if(p->alias().compare(outPlugAlias) == 0)
        {
            pcd->_outPlug = p;
            pcd->_outPlugAlias = outPlugAlias;
            outFound = true;
        }

        if(p->alias().compare(inPlugAlias) == 0)
        {
            pcd->_inPlug = p;
            pcd->_inPlugAlias = inPlugAlias;
            inFound = true;
        }

        if(inFound && outFound)
            break;
    }

    if(inFound && outFound)
        connectPlugins(pcd);
    else
        delete pcd;
}

bool PluginLoader::disconnectPlugin(PluginConnData *pcd)
{
    QMutexLocker locker(&_pcdMutex);
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
    QMutexLocker locker(&_pcdMutex);
    foreach (PluginConnData* pcd, _pcdList) {
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

    QMutexLocker locker(&_basePluginMutex);
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
    qDebug() << tr("Plugin configuration saved...") << Q_FUNC_INFO;
}

void PluginLoader::loadPrevConfig()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
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
            emit errMsg(tr("Plugin loading from previous configuration failed. %1 not found").arg(fName));
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

        /////////////////////////////////////////////
        // Lock on loaded plugins
        ////////////////////////////////////////////
        _loadedPluginsMutex.lock();

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

        ////////////////////////////////////////////
        // UNLOCK
        ///////////////////////////////////////////
        _loadedPluginsMutex.unlock();

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
    QMutexLocker locker(&_loadedPluginsMutex);
    foreach (NoobaPlugin* p, _loadedPlugins) {
        p->release();
        p->init();        
    }
}

bool PluginLoader::versionCheckOk(NoobaPluginAPIBase *apiBase, const QString& filename, QString& errStr)
{
    // NOTE: PRIVATE function. NOT Thread safe call with caution
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
    QMutexLocker locker(&_pcdMutex);
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
    // NOTE: PRIVATE function. NOT Thread safe call with caution
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
    // NOTE: PRIVATE function. NOT Thread safe call with caution
    // release then unload
    plugin->release();
    emit pluginAboutToUnloaded(plugin->alias());  // this should be called after release();
    delete plugin;
}

QString PluginLoader::getPluginAlias(const QString& pluginName)
{
    // NOTE: PRIVATE function. NOT Thread safe call with caution
    /*int count = 1;
    foreach(NoobaPlugin* p, _loadedPlugins)
    {
        if(p->getPluginInfo().name().compare(pluginName) == 0)
            count++;
    }
    return QString("%1[%2]").arg(pluginName).arg(count)*/;
    return pluginName;
}

void PluginLoader::updatePluginConnection(PluginConnData *pcd, bool isConnect)
{
    // NOTE: PRIVATE function. NOT Thread safe call with caution
    if(isConnect)
    {
        connect(pcd->_outPlug, SIGNAL(outputData(PluginPassData)), pcd->_inPlug, SLOT(inputData(PluginPassData)));
        connect(pcd->_outPlug, SIGNAL(outputData(QStringList,QList<QImage>)), pcd->_inPlug, SLOT(inputData(QStringList,QList<QImage>)));
        emit pluginsConnected(pcd);
    }
    else
    {
        disconnect(pcd->_outPlug, SIGNAL(outputData(PluginPassData)), pcd->_inPlug, SLOT(inputData(PluginPassData)));
        disconnect(pcd->_outPlug, SIGNAL(outputData(QStringList,QList<QImage>)), pcd->_inPlug, SLOT(inputData(QStringList,QList<QImage>)));
        emit pluginsDisconnected(pcd);
    }
    return;
}
