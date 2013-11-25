#include "CameraView.h"
#include "ui_CameraView.h"
#include "SharedImageBuffer.h"
#include "CaptureThread.h"
#include "ProcessingThread.h"
#include "PluginLoader.h"
#include "NoobaPlugin.h"
#include "StatPanel.h"
#include "OutputWind.h"
#include "ParamConfigWind.h"
#include "PluginsConfigUI.h"

// Qt
#include  <QMessageBox>
#include <QFileDialog>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif
#include <QMdiSubWindow>
#include <QMetaType>
#include <QDebug>
#include <QMutexLocker>
#include <QFile>

CameraView::CameraView(SharedImageBuffer *sharedImageBuffer, QWidget *parent) :
    QWidget(parent),
    _isWebCam(false),
    ui(new Ui::CameraView),
    _sharedImageBuffer(sharedImageBuffer),
    _captureThread(NULL),
    _deviceNumber(-1),
    _imageBufferSize(DEFAULT_IMAGE_BUFFER_SIZE),
    _inputWind(tr("Video Input")),
    _debugOutWind(new OutputWind()),
    _paramConfigUI(new ParamConfigWind()),
    _statPanel(new StatPanel)
{
    _vidState = nooba::StoppedState;
    ui->setupUi(this);
    ui->TileviewButton->setCheckable(true);
    setWindowTitle(tr("Camera View"));
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
    return;
}

CameraView::~CameraView()
{
    // Stop processing thread
    if(_processingThread && _processingThread->isRunning())
        stopProcessingThread();
    // Stop capture thread
    if(_captureThread && _captureThread->isRunning())
        stopCaptureThread();

    // Automatically start frame processing (for other streams)
    if(_sharedImageBuffer->isSyncEnabledForDeviceNumber(_deviceNumber))
        _sharedImageBuffer->setSyncEnabled(true);

    // Remove from shared buffer
    _sharedImageBuffer->removeByDeviceNumber(_deviceNumber);
    // Disconnect camera
    if(_captureThread && _captureThread->disconnectCaptureDevice())
        qDebug() << "[" << _deviceNumber << "] Capture device successfully disconnected.";
    else
        qDebug() << "[" << _deviceNumber << "] WARNING: capture device already disconnected.";

    QMap<QString, QMap<QString, MdiSubWindData* > >::iterator i = _frameViewerMap.begin();
    for(;i != _frameViewerMap.end(); i++)
    {
        foreach(MdiSubWindData* d, i.value())
        {
            delete d->_mdiSubWind;
            delete d;
        }
    }
    _frameViewerMap.clear();

    _debugOutWind->deleteLater();
    _paramConfigUI->deleteLater();
    _statPanel->deleteLater();
    delete ui;
    return;
}

bool CameraView::connectToCamera()
{
    _deviceNumber = 0;
    setupSharedBufferForNewDevice();

    setWindowTitle(tr("Camera View [ %1 ]").arg(_deviceNumber));
    _statPanel->setImageBufferBarSize(0, _sharedImageBuffer->getByDeviceNumber(_deviceNumber)->maxSize());

    _captureThread.reset(new CaptureThread(_sharedImageBuffer, _deviceNumber,true));
    _processingThread.reset(new ProcessingThread(_sharedImageBuffer, _deviceNumber));
    if(!_captureThread->connectToCamera())
    {
        QMessageBox errMsg;
        errMsg.setText(tr("Failed to open web-cam"));
        errMsg.setIcon(QMessageBox::Critical);
        errMsg.exec();
        return false;
    }

    _statPanel->setDeviceName(QString::number(_deviceNumber));
    addMDISubWindow(&_inputWind, true);
    connectThreadSignalSlots();
    _isWebCam = true;
    ui->prevButton->setDisabled(true); // disable on web cam mode, irrelavant
    _params.setFrameId(-1);
    _processingThread->start(QThread::HighPriority);
    return true;
}

bool CameraView::connectToVideoFileStream()
{
    QString startLocation;
#if QT_VERSION >= 0x050000
    startLocation = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#else
    startLocation = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#endif
    QString path = QFileDialog::getOpenFileName(this, tr("Open file"), startLocation);
    if(path.isEmpty())
        return false;

    _deviceNumber = 1000;
    setWindowTitle(tr("File Stream"));
    _isWebCam = false;
    setupSharedBufferForNewDevice();
    _statPanel->setDeviceName(tr("File mode"));
    _captureThread.reset(new CaptureThread(_sharedImageBuffer, _deviceNumber,true));
    _processingThread.reset(new ProcessingThread(_sharedImageBuffer, _deviceNumber));
    if(_captureThread->connectToFileStream(path))
    {
        addMDISubWindow(&_inputWind, true);
        connectThreadSignalSlots();
        _processingThread->start(QThread::HighPriority);

        return true;
    }
    return false;
}

void CameraView::stopCaptureThread()
{
    if(!_captureThread)
        return;

    qDebug() << "[" << _deviceNumber << "] About to stop capture thread...";
    _captureThread->stop();
    _sharedImageBuffer->wakeAll(); // This allows the thread to be stopped if it is in a wait-state
    // Take one frame off a FULL queue to allow the capture thread to finish
    if(_sharedImageBuffer->getByDeviceNumber(_deviceNumber)->isFull())
        _sharedImageBuffer->getByDeviceNumber(_deviceNumber)->get();
    _captureThread->wait(200);
    qDebug() << "[" << _deviceNumber << "] Capture thread successfully stopped.";

}

void CameraView::stopProcessingThread()
{
    if(!_processingThread)
        return;
    _sharedImageBuffer->wakeAll(); // This allows the thread to be stopped if it is in a wait-state
    _processingThread->wait(200);
    _processingThread->quit();  // exit event loop
    return;
}

void CameraView::connectThreadSignalSlots()
{
    connect(_processingThread.data(), SIGNAL(pluginLoaded(NoobaPlugin*)), this, SLOT(onPluginLoad(NoobaPlugin*)));
    connect(_processingThread.data(), SIGNAL(pluginInitialised(NoobaPlugin*)), this, SLOT(onPluginInitialised(NoobaPlugin*)));
    connect(_processingThread.data(), SIGNAL(pluginAboutToRelease(QString)), this, SLOT(onPluginAboutToRelease(QString)));
    connect(_processingThread.data(), SIGNAL(pluginAboutToUnload(QString)), this, SLOT(onPluginAboutToUnload(QString)));
    connect(_processingThread.data(), SIGNAL(createFrameViewer(QString, bool, NoobaPlugin*)), this, SLOT(onCreateFrameViewerRequest(QString, bool, NoobaPlugin*)));
    connect(_processingThread.data(), SIGNAL(debugMsg(QString)), _debugOutWind, SLOT(onDebugMsg(QString)));
    connect(_processingThread.data(), SIGNAL(createLineParamRequest(QString,QString,QColor,NoobaPlugin*)), this, SLOT(onCreateLineParamRequest(QString,QString,QColor,NoobaPlugin*)));
//    connect(_processingThread.data(), SIGNAL(inputFrame(QImage)), this, SLOT(onInputFrameUpdate(QImage)));
    connect(_captureThread.data(), SIGNAL(inputFrame(QImage)), this, SLOT(onInputFrameUpdate(QImage)));
    connect(_processingThread.data(), SIGNAL(updateStatisticsInGUI(ThreadStatisticsData)), this, SLOT(updateProcessingThreadStats(ThreadStatisticsData)));
    connect(_captureThread.data(), SIGNAL(updateStatisticsInGUI(ThreadStatisticsData)), this, SLOT(updateCaptureThreadStats(ThreadStatisticsData)));
    connect(_captureThread.data(), SIGNAL(endFileOfStream()), this, SLOT(onFileStreamEOF()));
    connect(_captureThread.data(), SIGNAL(frameAddedToImageBuffer()), _processingThread.data(), SIGNAL(FrameAddedToImageBuffer()));
    return;
}

void CameraView::setupSharedBufferForNewDevice()
{
    Buffer<cv::Mat> *imgBuffer = new Buffer<cv::Mat>(_imageBufferSize);
    bool addWithSync = false;
    _sharedImageBuffer->add(_deviceNumber, imgBuffer, addWithSync);
    _sharedImageBuffer->setSyncEnabled(true);

    if(_sharedImageBuffer->isSyncEnabledForDeviceNumber(_deviceNumber))
        qDebug() << tr("Camera connected. Waiting...") << Q_FUNC_INFO;
    else
        qDebug() << tr("Connecting to camera...") << Q_FUNC_INFO;
    return;
}

QMdiSubWindow *CameraView::addMDISubWindow(FrameViewer *frameViewer, bool isVisible)
{
    QMdiSubWindow *mdiWind = ui->mdiArea->addSubWindow(frameViewer);
    mdiWind->setContentsMargins(0,0,0,0);
    mdiWind->setVisible(isVisible);
    frameViewer->setMdiSubWindow(mdiWind);
    if(ui->TileviewButton->isChecked())
        ui->mdiArea->tileSubWindows();
    return mdiWind;
}

void CameraView::on_controlButton_clicked()
{
    if(_vidState == nooba::PlayingState)
    {
        setVideoState(nooba::PausedState);
        return;
    }

    setVideoState(nooba::PlayingState);
    return;
}

void CameraView::configurePlugins()
{
    if(_processingThread.isNull())
    {
        qDebug() << tr("Processing thread not initialised...") << Q_FUNC_INFO;
                return;
    }
    PluginLoader* pl = _processingThread->getPluginLoader();
    if(!pl)
    {
        qDebug() <<  tr("PluginLoader not initialised. Cant configure plugins until PluginLoader is initialised") << Q_FUNC_INFO;
        return;
    }

    if(_vidState == nooba::PlayingState)
        setVideoState(nooba::PausedState);

    PluginsConfigUI pluginConfUi(*pl);
    pluginConfUi.exec();

    //    setVideoState(state);
}

void CameraView::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    setVideoState(nooba::PausedState);
}

void CameraView::onInputFrameUpdate(const QImage &in)
{
    _inputWind.updateFrame(in);
}
void CameraView::onFileStreamEOF()
{
    setVideoState(nooba::StoppedState);
}

void CameraView::onPluginLoad(NoobaPlugin *plugin)
{
    connect(plugin, SIGNAL(generateAlert(QString,QString,nooba::AlertType,NoobaPlugin*)), this, SLOT(onGenerateAlert(QString,QString,nooba::AlertType,NoobaPlugin*)));
    _frameViewerMap.insert(plugin->alias(), QMap<QString, MdiSubWindData* >());
    return;
}

void CameraView::onPluginAboutToUnload(QString alias)
{
    QMap<QString, MdiSubWindData*> subWindMap = _frameViewerMap.value(alias);

    foreach (MdiSubWindData* d, subWindMap)
    {
        delete d->_mdiSubWind;
        delete d;
    }
    _frameViewerMap.remove(alias);
    return;
}

void CameraView::onPluginInitialised(NoobaPlugin *plugin)
{
    _paramConfigUI->addPlugin(plugin);
    return;
}

void CameraView::onPluginAboutToRelease(QString alias)
{
    _paramConfigUI->removePlugin(alias);
    return;
}

void CameraView::onCreateFrameViewerRequest(const QString &title, bool isVisible, NoobaPlugin* plugin)
{
    FrameViewer *fv = new FrameViewer(title, this);
    QMdiSubWindow* sw = addMDISubWindow(fv, isVisible);
    MdiSubWindData* data = new MdiSubWindData(plugin->alias(), sw, fv);
    connect(plugin, SIGNAL(updateFrameViewer(QString, QString,QImage)), this, SLOT(onFrameViewerUpdate(QString, QString,QImage)));
    connect(plugin, SIGNAL(setFrameViewerVisibility(QString,QString,bool)), this, SLOT(onFrameSetVisibiliy(QString, QString, bool)));

    _frameViewerMap[plugin->alias()].insert(title, data);
    return;
}

void CameraView::onFrameViewerUpdate(const QString &pluginAlias ,const QString &title, const QImage &frame)
{
    _pluginUpdateMutex.lock();
    MdiSubWindData* d = _frameViewerMap.value(pluginAlias).value(title);
    _pluginUpdateMutex.unlock();
    if(d)
    {
        d->_frameViewer->updateFrame(frame);
    }
    else
    {
        qDebug() << tr("frame viewer with title \'%1\' is not registered. use \"createFrameViewer()\" function"
                       " to register a frame viewer for the %2 plugin").arg(title).arg(pluginAlias) << Q_FUNC_INFO;
    }
    return;
}

void CameraView::onFrameSetVisibiliy(const QString &alias, const QString &title, bool isVisible)
{
    _pluginUpdateMutex.lock();
    MdiSubWindData* d = _frameViewerMap.value(alias).value(title);
    _pluginUpdateMutex.unlock();
    if(d)
    {
        d->_frameViewer->setVisibility(isVisible);
        if(ui->TileviewButton->isChecked())
            ui->mdiArea->tileSubWindows();
    }
    else
    {
        qDebug() << tr("frame viewer with title \'%1\' is not registered. use \"createFrameViewer()\" function"
                       " to register a frame viewer for the %2 plugin").arg(title).arg(alias) << Q_FUNC_INFO;
    }
    return;
}

void CameraView::updateCaptureThreadStats(ThreadStatisticsData statData)
{
    // Show [number of images in buffer / image buffer size] in imageBufferLabel
    _statPanel->setImageBufferLabel(QString("[")+QString::number(_sharedImageBuffer->getByDeviceNumber(_deviceNumber)->size())+
                                  QString("/")+QString::number(_sharedImageBuffer->getByDeviceNumber(_deviceNumber)->maxSize())+QString("]"));
    // Show percentage of image bufffer full in imageBufferBar
    _statPanel->setImageBufferBarValue(_sharedImageBuffer->getByDeviceNumber(_deviceNumber)->size());

    // Show processing rate in captureRateLabel
    _statPanel->setCaptureRateLabel(QString::number(statData.averageFPS)+" fps");
    // Show number of frames captured in nFramesCapturedLabel
    _statPanel->setnFramesCapturedLabel(QString("[") + QString::number(statData.nFramesProcessed) + QString("]"));
    return;
}

void CameraView::updateProcessingThreadStats(ThreadStatisticsData statData)
{
    // Show processing rate in processingRateLabel
    _statPanel->setProcessingRate(QString::number(statData.averageFPS)+" fps");
    // Show number of frames processed in nFramesProcessedLabel
    _statPanel->setnFramesProcessedLabel(QString("[") + QString::number(statData.nFramesProcessed) + QString("]"));
    return;
}

void CameraView::onCreateLineParamRequest(const QString &varName, const QString &frameViewerTitle, QColor lineColor, NoobaPlugin* plugin)
{
    MdiSubWindData *d =  _frameViewerMap.value(plugin->alias()).value(frameViewerTitle, NULL);
    if(!d)
    {
        qDebug() << tr("Line Parameter not created. frame viewer [ %1 ] is not registered.").arg(frameViewerTitle) << Q_FUNC_INFO;
        return;
    }

    d->_frameViewer->createLineParam(varName, lineColor);

    connect(d->_frameViewer, SIGNAL(lineParamChanged(QString,QString,QLine)),
            plugin, SLOT(onLineParamUpdate(QString,QString,QLine)), Qt::UniqueConnection);  // same plugin and frame viewer will be connected multiple times
}

void CameraView::onGenerateAlert(const QString &frameViewerTitle, const QString& msg, nooba::AlertType type, NoobaPlugin* plugin)
{
    MdiSubWindData* d = _frameViewerMap.value(plugin->alias()).value(frameViewerTitle, NULL);
    if(!d)
        return;

    d->_frameViewer->triggerAlert(type);
}

void CameraView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    if(ui->TileviewButton->isChecked())
        ui->mdiArea->tileSubWindows();
}

QImage CameraView::cvt2QImage(const Mat &cvImg)
{
    QImage img;
    if(cvImg.channels() == 1)
    {
        img = QImage((const unsigned char*)(cvImg.data),
                     cvImg.cols,cvImg.rows,cvImg.step,  QImage::Format_Indexed8);
    }
    else
    {
        img = QImage((const unsigned char*)(cvImg.data),
                     cvImg.cols,cvImg.rows,cvImg.step,  QImage::Format_RGB888);

    }
    return img;
}

void CameraView::setVideoState(nooba::VideoState state)
{
    switch(state){

    case nooba::StoppedState:
    {
        _vidState = nooba::StoppedState;
        ui->controlButton->setIcon(QIcon(":/Resources/super-mono-iconset/button-play.png"));
        ui->controlButton->setToolTip(tr("Play"));
        stopCaptureThread();
        break;
    }
    case nooba::PausedState:
    {
        _vidState = nooba::PausedState;
        ui->controlButton->setIcon(QIcon(":/Resources/super-mono-iconset/button-play.png"));
        ui->controlButton->setToolTip(tr("Play"));
        stopCaptureThread();
        break;
    }
    case nooba::PlayingState:
    {
        _vidState = nooba::PlayingState;
        ui->controlButton->setIcon(QIcon(":/Resources/super-mono-iconset/button-pause.png"));
        ui->controlButton->setToolTip(tr("Pause"));
        _captureThread->start(QThread::NormalPriority);
        break;
    }
    default:
        break;
    }
    _params.setVidState(_vidState);
    return;
}
