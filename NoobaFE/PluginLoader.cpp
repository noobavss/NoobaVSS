#include "PluginLoader.h"
#include "intproperty.h"

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
    m_activePlugin(NULL),
    m_Dir(QApplication::instance()->applicationDirPath())
{
    m_Dir.cd("plugins");    // look in plugins directory
    // init plugin loader
    loadPluginInfo();
    loadPlugin(prevLoadedPluginfileName());
}

PluginLoader::~PluginLoader()
{
    if(m_activePlugin)
        m_activePlugin->release();

    saveSettings(m_activePluginFileName);
}


int PluginLoader::loadPluginInfo()
{
    /*QDir pluginsDir(QApplication::instance()->applicationDirPath());*/
    pluginInfoList.clear();
    int pluginCount = 0;

    int api_majorVer;
    int api_minorVer;
    QString errStr;

    // go through the directory and try to load the plugin files
    foreach (QString fileName, m_Dir.entryList(QDir::Files))
    {        
        QPluginLoader pluginLoader(m_Dir.absoluteFilePath(fileName));       // use a scoped pluginLoader (separate from the m_QPluginLoader ) instance so that loading 
                                                                            // plugins to retrieve information and unloading doesn't affect the currently loaded plugin
        QObject *plugin = pluginLoader.instance();
        if (!plugin)
            break;

        NoobaPluginAPIBase* apiBase = qobject_cast<NoobaPluginAPIBase *>(plugin);
        if (!apiBase)
            break;

        api_majorVer = apiBase->APIMajorVersion();
        api_minorVer = apiBase->APIMinorVersion();
        bool ok = true;
        if(api_majorVer > API_MAJOR_VERSION)
            ok = false;

        if(api_majorVer == API_MAJOR_VERSION && api_minorVer > API_MINOR_VERSION)
            ok = false;

        if(!ok)
        {
            errStr.append( tr("\n%1 version(%2.%3) is newer than the program version (%4.%5). Plugin not loaded ").
                  arg(fileName).arg(api_majorVer).arg(api_minorVer).arg(API_MAJOR_VERSION).arg(API_MINOR_VERSION));
            break;
        }

        // add the plugin details to the list
        NoobaPluginAPI* api = qobject_cast<NoobaPluginAPI* >(plugin);
        nooba::PluginData pluginData(fileName, api->getPluginInfo(),api_majorVer, api_minorVer);
        pluginInfoList.append(pluginData);
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

NoobaPluginAPI *PluginLoader::loadPlugin( const QString fileName )
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


    m_QPluginLoader.setFileName(m_Dir.absoluteFilePath(fileName));
    QObject *plugin = m_QPluginLoader.instance();

    if (plugin)
    {
        NoobaPluginAPI* apiBase = qobject_cast<NoobaPluginAPI* >(plugin);
        if (apiBase)
        {
            m_activePlugin = apiBase;
            m_activePluginFileName = fileName;
            m_activePlugin->init();
            //TODO: Code crash property
//            QList<Property *> lst = apiBase->getPropertyList();
//            foreach(Property *p, lst)
//            {
//                if( p->getType() == intProperty)
//                {
////                    IntProperty* ip =(IntProperty *)p;
////                    qDebug() << ip->getName() << "\nMax " << ip->getMaxValue() << "\nMin";

//                }
//            }

            return apiBase; // return plugin, successful
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
    if(m_activePlugin) m_activePlugin->release();

    m_QPluginLoader.unload();
    m_activePlugin = NULL;
    m_activePluginFileName.clear();
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
