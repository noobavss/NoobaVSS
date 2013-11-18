
#include <QDebug>
#include <QPluginLoader>
#include <QSettings>

#include <NoobaEye.h>
#include <noobapluginapi.h>
#include "NoobaPlugin.h"

NoobaPlugin::NoobaPlugin(const QString &fileName, const QString& alias, NoobaPluginAPI *api, QPluginLoader *loader, QObject *parent):
    QObject(parent),
    _isBasePlugin(false),
    _api(api),
    _pluginLoader(loader),
    _fileName(fileName),
    _alias(alias)
{
}

NoobaPlugin::~NoobaPlugin()
{
    release();
    _pluginLoader->unload();
    delete _pluginLoader;    
}

bool NoobaPlugin::init()
{
    // TODO: Need to load previous configurations from QSettings
    qDebug() << _alias << " about to initialise " << Q_FUNC_INFO;
    initSignalSlots();
    bool ok =  _api->init();
    if(!ok)
        qDebug() << tr("plugin: '%1' initialisation failed!").arg(_alias) << Q_FUNC_INFO;
    else
        qDebug() << _alias << " initialised successfully " << Q_FUNC_INFO;
    emit onInit(this);
    return ok;
}

bool NoobaPlugin::release()
{
    qDebug() << _alias << " release " << Q_FUNC_INFO;
    emit onAboutToRelease(this);
    // TODO need to save configurations to a QSettings
    releaseSignalSlots();
    deleteMapItems<IntData* >(_intMap);
    deleteMapItems<DoubleData* >(_doubleMap);
    deleteMapItems<StringData* >(_stringMap);
    deleteMapItems<StringListData* >(_stringListMap);
    deleteMapItems<PointData* >(_pointMap);
    deleteMapItems<RectData* >(_rectMap);

    bool ok = _api->release();
    if(!ok)
        qDebug() << tr("plugin '%1' releasing failed.") << Q_FUNC_INFO;

    return ok;
}

bool NoobaPlugin::procFrame(const cv::Mat &in, cv::Mat &out, ProcParams &params)
{
    return _api->procFrame(in, out, params);
}

PluginInfo NoobaPlugin::getPluginInfo() const
{
    return _api->getPluginInfo();
}

void NoobaPlugin::onCreateIntParam(const QString &varName, int val, int max, int min)
{
    _intMap.insert(varName, new IntData(varName, val, max, min));
}

void NoobaPlugin::onCreateDoubleParam(const QString &varName, double val, double max, double min)
{
    _doubleMap.insert(varName, new DoubleData(varName, val, max, min));
}

void NoobaPlugin::onCreateStringParam(const QString &varName, const QString &val, bool isFilePath)
{
    _stringMap.insert(varName,new StringData(varName, val, isFilePath));
}

void NoobaPlugin::onCreateMultiValParam(const QString &varName, const QStringList &varList)
{
    _stringListMap.insert(varName, new StringListData(varName, varList));
}

void NoobaPlugin::onCreatePointParam(const QString &varName, const QPointF &val)
{
    Q_UNUSED(varName)
    Q_UNUSED(val)
}

void NoobaPlugin::onCreateRectParam(const QString &varName, const QRectF &val)
{
    Q_UNUSED(varName)
    Q_UNUSED(val)
}

void NoobaPlugin::onDebugMsg(const QString &msg)
{
    qDebug() << "Debug MSG: " << msg;
}

void NoobaPlugin::initSignalSlots()
{
    connect(_api, SIGNAL(debugMsgRequest(QString)), this, SIGNAL(debugMsg(QString)));
    connect(_api, SIGNAL(debugMsgRequest(QString)), this, SLOT(onDebugMsg(QString)));
    connect(_api, SIGNAL(createIntParamRequest(QString,int,int,int)), this, SLOT(onCreateIntParam(QString,int, int, int)));
    connect(_api, SIGNAL(createDoubleParamRequest(QString,double, double, double)), this, SLOT(onCreateDoubleParam(QString,double, double, double)));
    connect(_api, SIGNAL(createStringParamRequest(QString,QString, bool)), this, SLOT(onCreateStringParam(QString,QString, bool)));
    connect(_api, SIGNAL(createMultiValParamRequest(QString,QStringList)), this, SLOT(onCreateMultiValParam(QString,QStringList)));
    connect(_api, SIGNAL(createPointParamRequest(QString,QPointF)), this, SLOT(onCreatePointParam(QString,QPointF)));
    connect(_api, SIGNAL(createRectParamRequest(QString,QRectF)), this, SLOT(onCreateRectParam(QString,QRectF)));

    qRegisterMetaType<PluginPassData>("PluginPassData");
    connect(_api, SIGNAL(outputDataRequest(PluginPassData)), this, SIGNAL(outputData(PluginPassData)));
    qRegisterMetaType< QList<QImage> >("QList<QImage>");
    connect(_api, SIGNAL(outputDataRequest(QStringList,QList<QImage>)), this, SIGNAL(outputData(QStringList,QList<QImage>)));
    connect(_api, SIGNAL(createFrameViewerRequest(QString)), this, SIGNAL(createFrameViewer(QString)));
    connect(_api, SIGNAL(updateFrameViewerRequest(QString,QImage)), this, SIGNAL(updateFrameViewer(QString,QImage)));
}

void NoobaPlugin::onIntParamUpdate(const QString &varName, int val)
{
    _intMap.value(varName, 0)->_val = val;
    _api->onIntParamChanged(varName, val);
}

void NoobaPlugin::onDoubleParamUpdate(const QString &varName, double val)
{
    _doubleMap.value(varName, 0)->_val = val;
    _api->onDoubleParamChanged(varName, val);
}

void NoobaPlugin::onStringParamUpdate(const QString &varName, const QString &val)
{
    _stringMap.value(varName)->_val = val;
    _api->onStringParamChanged(varName, val);
}

void NoobaPlugin::onMultiValParamUpdate(const QString &varName, const QString &val)
{
    _stringListMap.value(varName)->_val = val;
    _api->onMultiValParamChanged(varName, val);
}

void NoobaPlugin::onPointParamUpdate(const QString &varName, const QPointF &val)
{
    PointData* p = _pointMap.value(varName);
    if(!p)
        return;

    p->_val.setX(val.x());
    p->_val.setY(val.y());
    _api->onPointParamChanged(varName, val);
}

void NoobaPlugin::onRectParamUpdate(const QString &varName, const QRectF &val)
{
    _rectMap.value(varName)->_val.setRect(val.x(), val.y(),val.width(), val.height());
    _api->onRectParamChanged(varName, val);
}

void NoobaPlugin::inputData(const PluginPassData& data)
{
    _api->inputData(data);
}

void NoobaPlugin::inputData(const QStringList &strList, QList<QImage> imageList)
{
    _api->inputData(strList, imageList);
}

void NoobaPlugin::saveConfig(const QString &filename)
{
    QSettings s;
//    if(filename.isEmpty())
//    {
//        s = QSettings(nooba::Organisation, nooba::ProgramName);
//    }
//    else
//    {
//        s = QSettings(filename, QSettings::IniFormat);
//    }
}

void NoobaPlugin::loadPrevConfig()
{
}

void NoobaPlugin::releaseSignalSlots()
{
    _api->disconnect();
}

template <typename value>
void NoobaPlugin::deleteMapItems(QMap<QString, value>& map)
{
    typename QMap<QString, value>::iterator iter = map.begin();
    for(; iter != map.end(); iter++)
    {
        delete iter.value();
    }
    map.clear();
}
