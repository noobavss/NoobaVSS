
// Qt
#include <QDebug>
#include <QPluginLoader>
#include <QSettings>
#include <QSignalMapper>

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
    emit onAboutToRelease(_alias);
    // TODO need to save configurations to a QSettings
    releaseSignalSlots();
    deleteMapItems<IntData* >(_intMap);
    deleteMapItems<DoubleData* >(_doubleMap);
    deleteMapItems<StringData* >(_stringMap);
    deleteMapItems<StringListData* >(_stringListMap);
    deleteMapItems<PointData* >(_pointMap);
    deleteMapItems<RectData* >(_rectMap);
    deleteMapItems<LineData* >(_lineMap);
    deleteMapItems<FrameViewerData* >(_frameViewerDataMap);
    deleteMapItems<FilePathData* >(_filePathDataMap);

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

void NoobaPlugin::onCreateFilePathParam(const QString &varName, QString path, nooba::PathType pathType, const QString &filter)
{
    _filePathDataMap.insert(varName, new FilePathData(varName, path, filter, pathType));
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

void NoobaPlugin::onCreateLineParam(const QString &varName, const QString &frameViewerTitle)
{
    _lineMap.insert(varName + frameViewerTitle, new LineData(varName, frameViewerTitle));
    emit createLineParam(varName, frameViewerTitle, this);
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

void NoobaPlugin::onUpdateFrameViewerRequest(const QString &title, const QImage &frame)
{
    // this intermediate map is used to store a copy of the image to pass the image to
    // GUI thread without loosing the content;
    _frameViewerDataMap.value(title)->_img = frame.copy();
    emit updateFrameViewer(_alias, title, _frameViewerDataMap.value(title)->_img);
}

void NoobaPlugin::onSetFrameViewerVisibility(const QString &title, bool isVisible)
{
    _frameViewerDataMap.value(title)->_isVisible = isVisible;
    emit setFrameViewerVisibility(_alias, title, isVisible);
}

void NoobaPlugin::initSignalSlots()
{
    connect(_api, SIGNAL(debugMsgRequest(QString)), this, SIGNAL(debugMsg(QString)));
    connect(_api, SIGNAL(debugMsgRequest(QString)), this, SLOT(onDebugMsg(QString)));
    connect(_api, SIGNAL(createIntParamRequest(QString,int,int,int)), this, SLOT(onCreateIntParam(QString,int, int, int)));
    connect(_api, SIGNAL(createDoubleParamRequest(QString,double, double, double)), this, SLOT(onCreateDoubleParam(QString,double, double, double)));
    connect(_api, SIGNAL(createStringParamRequest(QString,QString, bool)), this, SLOT(onCreateStringParam(QString,QString, bool)));
    connect(_api, SIGNAL(createMultiValParamRequest(QString,QStringList)), this, SLOT(onCreateMultiValParam(QString,QStringList)));
//    connect(_api, SIGNAL(createPointParamRequest(QString,QPointF)), this, SLOT(onCreatePointParam(QString,QPointF)));
//    connect(_api, SIGNAL(createRectParamRequest(QString,QRectF)), this, SLOT(onCreateRectParam(QString,QRectF)));
    connect(_api, SIGNAL(createFrameViewerRequest(QString,bool)), this, SLOT(onCreateFrameViewer(QString,bool)));
    connect(_api, SIGNAL(createFilePathParamRequest(QString,QString,nooba::PathType,QString)),
            this, SLOT(onCreateFilePathParam(QString,QString,nooba::PathType,QString)));

    connect(_api, SIGNAL(createLineParamRequest(QString,QString)), this, SLOT(onCreateLineParam(QString,QString))); // after version 0.10

    qRegisterMetaType<PluginPassData>("PluginPassData");
    connect(_api, SIGNAL(outputDataRequest(PluginPassData)), this, SIGNAL(outputData(PluginPassData)));
    qRegisterMetaType< QList<QImage> >("QList<QImage>");
    connect(_api, SIGNAL(outputDataRequest(QStringList,QList<QImage>)), this, SIGNAL(outputData(QStringList,QList<QImage>)));
    connect(_api, SIGNAL(updateFrameViewerRequest(QString,QImage)), this, SLOT(onUpdateFrameViewerRequest(QString,QImage)));
    connect(_api, SIGNAL(updateFrameViewerVisibilityRequest(QString,bool)), this, SLOT(onSetFrameViewerVisibility(QString, bool)));
    connect(this, SIGNAL(intParamUpdate(QString,int)), _api, SLOT(onIntParamChanged(QString,int)));
    connect(this, SIGNAL(doubleParamUpdate(QString,double)), _api, SLOT(onDoubleParamChanged(QString,double)));
    connect(this, SIGNAL(stringParamUpdate(QString,QString)), _api, SLOT(onStringParamChanged(QString,QString)));
    connect(this, SIGNAL(filePathParamUpdate(QString,QString)), _api, SLOT(onFilePathParamChanged(QString,QString)));
    connect(this, SIGNAL(multiValParamUpdate(QString,QString)), _api, SLOT(onMultiValParamChanged(QString,QString)));
//    connect(this, SIGNAL(pointParamUpdate(QString,QPointF)), _api, SLOT(onPointParamChanged(QString,QPointF)));
//    connect(this, SIGNAL(rectParamUpdate(QString,QRectF)), _api, SLOT(onRectParamChanged(QString,QRectF)));
    connect(this, SIGNAL(lineParamUpdate(QString,QString,QLine)), _api, SLOT(onLineParamUpdated(QString,QString,QLine)));
}

void NoobaPlugin::onIntParamUpdate(const QString &varName, int val)
{
    _intMap.value(varName, 0)->_val = val;
    emit intParamUpdate(varName, val);
}

void NoobaPlugin::onDoubleParamUpdate(const QString &varName, double val)
{
    _doubleMap.value(varName, 0)->_val = val;
    emit doubleParamUpdate(varName, val);
}

void NoobaPlugin::onStringParamUpdate(const QString &varName, const QString &val)
{
    _stringMap.value(varName)->_val = val;
    emit stringParamUpdate(varName, val);
}

void NoobaPlugin::onFilePathParamUpdate(const QString &varName, const QString &path)
{
    FilePathData* fpd = _filePathDataMap.value(varName);
    if(!fpd)
    {
        qDebug() << tr("File path param '%1' not created").arg(varName) << Q_FUNC_INFO;
        return;
    }
    fpd->_val = path;
    emit filePathParamUpdate(varName, path);
}

void NoobaPlugin::onMultiValParamUpdate(const QString &varName, const QString &val)
{
    _stringListMap.value(varName)->_val = val;
    emit multiValParamUpdate(varName, val);
}

void NoobaPlugin::onPointParamUpdate(const QString &varName, const QPointF &val)
{
    PointData* p = _pointMap.value(varName);
    if(!p)
        return;

    p->_val.setX(val.x());
    p->_val.setY(val.y());
    emit pointParamUpdate(varName, val);
}

void NoobaPlugin::onRectParamUpdate(const QString &varName, const QRectF &val)
{
    _rectMap.value(varName)->_val.setRect(val.x(), val.y(),val.width(), val.height());
    emit rectParamUpdate(varName, val);
}

void NoobaPlugin::onLineParamUpdate(const QString &varName, const QString &frameViewerTitle, const QLine &line)
{
    qDebug() << sender()->objectName() << Q_FUNC_INFO;
    LineData *lineData = _lineMap.value(varName + frameViewerTitle);
    if(!lineData)
        return;

    lineData->_val = line;
    emit lineParamUpdate(varName, frameViewerTitle, line);
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

void NoobaPlugin::onCreateFrameViewer(const QString &title, bool isVisible)
{
    FrameViewerData* fvd = new FrameViewerData(title, isVisible);
    _frameViewerDataMap.insert(title, fvd);
    emit createFrameViewer(title, isVisible, this);
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
