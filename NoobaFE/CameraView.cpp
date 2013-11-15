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

// Qt
#include  <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMdiSubWindow>
#include <QMetaType>

CameraView::CameraView(SharedImageBuffer *sharedImageBuffer, QWidget *parent) :
    QWidget(parent),
    _isWebCam(false),
    ui(new Ui::CameraView),
    _sharedImageBuffer(sharedImageBuffer),
    _captureThread(NULL),
    _pluginLoader(new PluginLoader(this)),
    _deviceNumber(-1),
    _imageBufferSize(1),
    _inputWind(tr("Video Input")),
    _debugOutWind(new OutputWind()),
    _paramConfigUI(new ParamConfigWind()),
    _statPanel(new StatPanel)
{
    _vidState = nooba::StoppedState;
    ui->setupUi(this);
    setWindowTitle(tr("Camera View"));
    connectSignalSlots();
    _pluginLoader->loadPluginInfo();
    _pluginLoader->loadPrevConfig();
    return;
}

CameraView::~CameraView()
{
    // Stop processing thread
    if(_processingThread->isRunning())
        stopProcessingThread();
    // Stop capture thread
    if(_captureThread->isRunning())
        stopCaptureThread();

    // Automatically start frame processing (for other streams)
    if(_sharedImageBuffer->isSyncEnabledForDeviceNumber(_deviceNumber))
        _sharedImageBuffer->setSyncEnabled(true);

    // Remove from shared buffer
    _sharedImageBuffer->removeByDeviceNumber(_deviceNumber);
    // Disconnect camera
    if(_captureThread->disconnectCamera())
        qDebug() << "[" << _deviceNumber << "] Camera successfully disconnected.";
    else
        qDebug() << "[" << _deviceNumber << "] WARNING: Camera already disconnected.";

    QMap<NoobaPlugin*, QMap<QString, MdiSubWindData* > >::iterator i = _frameViewerMap.begin();
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

void CameraView::connectToCamera()
{
    _deviceNumber = 0;
    setupSharedBufferForNewDevice();

    _statPanel->setImageBufferBarSize(0, _sharedImageBuffer->getByDeviceNumber(_deviceNumber)->maxSize());

    _captureThread.reset(new CaptureThread(_sharedImageBuffer, _deviceNumber,true));
    _processingThread.reset(new ProcessingThread(_sharedImageBuffer, _pluginLoader.data(), _deviceNumber));
    if(!_captureThread->connectToCamera())
    {
        QMessageBox errMsg;
        errMsg.setText(tr("Failed to open web-cam"));
        errMsg.setIcon(QMessageBox::Critical);
        errMsg.exec();
        return;
    }

    _statPanel->setDeviceName(QString::number(_deviceNumber));
    addMDISubWindow(&_inputWind);
    qRegisterMetaType<struct ThreadStatisticsData>("ThreadStatisticsData");
    connect(_processingThread.data(), SIGNAL(inputFrame(QImage)), this, SLOT(onInputFrameUpdate(QImage)));
    connect(_processingThread.data(), SIGNAL(updateStatisticsInGUI(ThreadStatisticsData)), this, SLOT(updateProcessingThreadStats(ThreadStatisticsData)));
    connect(_captureThread.data(), SIGNAL(updateStatisticsInGUI(ThreadStatisticsData)), this, SLOT(updateCaptureThreadStats(ThreadStatisticsData)));
    _isWebCam = true;
    ui->prevButton->setDisabled(true); // disable on web cam mode, irrelavant
    _params.setFrameId(-1);
    return;
}

void CameraView::connectToVideoFileStream()
{
#if QT_VERSION >= 0x050000
    QString path = QFileDialog::getOpenFileName(this, tr("Open file"),
                                                QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
#else
    QString path = QFileDialog::getOpenFileName(this, tr("Open file"),
                                                QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));
#endif
    if(path.isEmpty())
        return;
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
    _captureThread->wait();
    qDebug() << "[" << _deviceNumber << "] Capture thread successfully stopped.";
    return;
}

void CameraView::stopProcessingThread()
{
    if(!_processingThread)
        return;
    qDebug() << "[" << _deviceNumber << "] About to stop processing thread...";
    _processingThread->stop();
    _sharedImageBuffer->wakeAll(); // This allows the thread to be stopped if it is in a wait-state
    _processingThread->wait();
    qDebug() << "[" << _deviceNumber << "] Processing thread successfully stopped.";
    return;
}

void CameraView::connectSignalSlots()
{
    connect(_pluginLoader.data(), SIGNAL(pluginLoaded(NoobaPlugin*)), this, SLOT(onPluginLoad(NoobaPlugin*)));
    connect(_pluginLoader.data(), SIGNAL(pluginInitialised(NoobaPlugin*)), this, SLOT(onPluginInitialised(NoobaPlugin*)));
    connect(_pluginLoader.data(), SIGNAL(pluginAboutToRelease(NoobaPlugin*)), this, SLOT(onPluginAboutToRelease(NoobaPlugin*)));
    connect(_pluginLoader.data(), SIGNAL(pluginAboutToUnloaded(NoobaPlugin*)), this, SLOT(onPluginAboutToUnload(NoobaPlugin*)));
    return;
}

void CameraView::initializeMdiArea()
{
    addMDISubWindow(&_inputWind);
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

QMdiSubWindow *CameraView::addMDISubWindow(FrameViewer *frameViewer)
{
    QMdiSubWindow *mdiWind = ui->mdiArea->addSubWindow(frameViewer);
    mdiWind->setContentsMargins(0,0,0,0);
    mdiWind->setVisible(true);
    return mdiWind;
}

void CameraView::on_controlButton_clicked()
{
    if(_vidState == nooba::PlayingState)
    {
        setVideoState(nooba::PausedState);
        return;
    }

//    double rate= _vidCapture.get(CV_CAP_PROP_FPS);

//    if(rate > 0)    // video file
//    {
//        _delay = 1000/rate;
//        _params.setFrameRate(rate);
//    }
//    else    // on web cam open
//    {
//        _delay = 50;   // assume 10 fps
//        _params.setFrameRate(20);
//    }

//    _timer.start(_delay);
    setVideoState(nooba::PlayingState);
    return;
}

void CameraView::onInputFrameUpdate(const QImage &in)
{
    _inputWind.updateFrame(in);
}

void CameraView::onPluginLoad(NoobaPlugin *plugin)
{
    connect(plugin, SIGNAL(debugMsg(QString)), _debugOutWind, SLOT(onDebugMsg(QString)));
    connect(plugin, SIGNAL(createFrameViewer(QString)), this, SLOT(onCreateFrameViewerRequest(QString)));
    _frameViewerMap.insert(plugin, QMap<QString, MdiSubWindData* >());
    return;
}

void CameraView::onPluginAboutToUnload(NoobaPlugin *plugin)
{
    QMap<QString, MdiSubWindData*> subWindMap = _frameViewerMap.value(plugin);

    foreach (MdiSubWindData* d, subWindMap)
    {
        delete d->_mdiSubWind;
        delete d;
    }
    _frameViewerMap.remove(plugin);
    return;
}

void CameraView::onPluginInitialised(NoobaPlugin *plugin)
{
    _paramConfigUI->addPlugin(plugin);
    return;
}

void CameraView::onPluginAboutToRelease(NoobaPlugin *plugin)
{
    _paramConfigUI->removePlugin(plugin);
    return;
}

void CameraView::onCreateFrameViewerRequest(const QString &title)
{
    NoobaPlugin* p = qobject_cast<NoobaPlugin*>(sender());  // get the sender of the signal
    if(!p)
        return;

    FrameViewer *fv = new FrameViewer(title, this);
    QMdiSubWindow* sw = addMDISubWindow(fv);
    MdiSubWindData* data = new MdiSubWindData(p, sw, fv);
    _frameViewerMap[p].insert(title, data);
    connect(p, SIGNAL(updateFrameViewer(QString,QImage)), this, SLOT(onFrameViewerUpdate(QString,QImage)));
    return;
}

void CameraView::onFrameViewerUpdate(const QString &title, const QImage &frame)
{
    NoobaPlugin* p = qobject_cast<NoobaPlugin*>(sender());  // get the sender of the signal
    if(!p)
        return;

    MdiSubWindData* d = _frameViewerMap.value(p).value(title);
    if(d)
    {
        d->_frameViewer->updateFrame(frame);
    }
    else
    {
        qDebug() << tr("frame viewer with title \'%1\' is not registered. use \"createFrameViewer()\" function"
                       " to register a frame viewer for the %2 plugin").arg(title).arg(p->alias()) << Q_FUNC_INFO;
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
        stopProcessingThread();
        break;
    }
    case nooba::PausedState:
    {
        _vidState = nooba::PausedState;
        ui->controlButton->setIcon(QIcon(":/Resources/super-mono-iconset/button-play.png"));
        ui->controlButton->setToolTip(tr("Play"));
        stopCaptureThread();
        stopProcessingThread();
        break;
    }
    case nooba::PlayingState:
    {
        _vidState = nooba::PlayingState;
        ui->controlButton->setIcon(QIcon(":/Resources/super-mono-iconset/button-pause.png"));
        ui->controlButton->setToolTip(tr("Pause"));
        _captureThread->start(QThread::NormalPriority);
        _processingThread->start(QThread::HighPriority);
        break;
    }
    default:
        break;
    }
    _params.setVidState(_vidState);
    return;
}
