
#include <QDebug>
#include <noobapluginapi.h>

#include "NoobaPlugin.h"

NoobaPlugin::NoobaPlugin(NoobaPluginAPI *api, QObject *parent):
    QObject(parent),
    _api(api)
{
}

NoobaPlugin::~NoobaPlugin()
{
    release();
}

bool NoobaPlugin::init()
{
    initSignalSlots();
    return _api->init();
}

bool NoobaPlugin::release()
{
    releaseSignalSlots();
    deleteMapItems<QMap<QString, IntData* > >(_intMap);
    deleteMapItems<QMap<QString, DoubleData* > >(_doubleMap);
    deleteMapItems<QMap<QString, StringData* > >(_stringMap);
    deleteMapItems<QMap<QString, StringListData* > >(_stringListMap);
    return _api->release();
}

bool NoobaPlugin::procFrame(const cv::Mat &in, cv::Mat &out, ProcParams &params)
{
    return _api->procFrame(in, out, params);
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
    if(!isFilePath)
    {
        _stringMap.insert(varName,new StringData(varName, val, isFilePath));
    }
}

void NoobaPlugin::onCreateMultiValParam(const QString &varName, const QStringList &varList)
{
    _stringListMap.insert(varName, new StringListData(varName, varList));
}

void NoobaPlugin::onDebugMsg(const QString &msg)
{
    qDebug() << "Debug MSG: " << msg;
}

void NoobaPlugin::initSignalSlots()
{
    connect(_api, SIGNAL(debugMsg(QString)), this, SIGNAL(debugMsg(QString)));
    connect(_api, SIGNAL(debugMsg(QString)), this, SLOT(onDebugMsg(QString)));
    connect(_api, SIGNAL(createIntParam(QString,int,int,int)), this, SLOT(onCreateIntParam(QString,int, int, int)));
    connect(_api, SIGNAL(createDoubleParam(QString,double, double, double)), this, SLOT(onCreateDoubleParam(QString,double, double, double)));
    connect(_api, SIGNAL(createStringParam(QString,QString, bool)), this, SLOT(onCreateStringParam(QString,QString, bool)));
    connect(_api, SIGNAL(createMultiValParam(QString,QStringList)), this, SLOT(onCreateMultiValParam(QString,QStringList)));
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

void NoobaPlugin::releaseSignalSlots()
{
    _api->disconnect();
}

template <typename Map>
void NoobaPlugin::deleteMapItems(Map map)
{
    typename Map::iterator iter = map.begin();
    for(; iter != map.end(); iter++)
    {
        delete iter.value();
    }
    map.clear();
}
