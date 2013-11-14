#include "CameraView.h"
#include "ui_CameraView.h"
#include "SharedImageBuffer.h"
#include "CaptureThread.h"
#include "PluginLoader.h"
#include "NoobaPlugin.h"

// Qt
#include  <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMdiSubWindow>

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
    _paramConfigUI(new ParamConfigWind()),
    _debugOutWind(new OutputWind())
{
    _vidState = nooba::StoppedState;
    ui->setupUi(this);
    setWindowTitle(tr("Camera View"));
    connectSignalSlots();
    _pluginLoader->loadPrevConfig();
    _pluginLoader->loadPluginInfo();
}

CameraView::~CameraView()
{
//    // Stop processing thread
//    if(processingThread->isRunning())
//        stopProcessingThread();
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

    delete ui;
}

void CameraView::connectToCamera()
{
    _deviceNumber = 0;
    Buffer<cv::Mat> *imgBuffer = new Buffer<cv::Mat>(_imageBufferSize);
    bool addWithSync = false;
    _sharedImageBuffer->add(_deviceNumber, imgBuffer, addWithSync);
    _sharedImageBuffer->setSyncEnabled(true);
    if(_sharedImageBuffer->isSyncEnabledForDeviceNumber(_deviceNumber))
        qDebug() << tr("Camera connected. Waiting...") << Q_FUNC_INFO;
    else
        qDebug() << tr("Connecting to camera...") << Q_FUNC_INFO;

    _captureThread.reset(new CaptureThread(_sharedImageBuffer, _deviceNumber,true));
    if(!_captureThread->connectToCamera())
    {
        QMessageBox errMsg;
        errMsg.setText(tr("Failed to open web-cam"));
        errMsg.setIcon(QMessageBox::Critical);
        errMsg.exec();
        return;
    }
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

QMdiSubWindow *CameraView::addMDISubWindow(FrameViewer *frameViewer)
{
    QMdiSubWindow *mdiWind = ui->mdiArea->addSubWindow(frameViewer);
    mdiWind->setContentsMargins(0,0,0,0);
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

void CameraView::onPluginLoad(NoobaPlugin *plugin)
{
    connect(plugin, SIGNAL(debugMsg(QString)), _debugOutWind.data(), SLOT(onDebugMsg(QString)));
    connect(plugin, SIGNAL(createFrameViewer(QString)), this, SLOT(onCreateFrameViewerRequest(QString)));
    _frameViewerMap.insert(plugin, QMap<QString, MdiSubWindData* >());
}

void CameraView::onPluginAboutToUnload(NoobaPlugin *plugin)
{
    QMap<QString, MdiSubWindData*> subWindMap = _frameViewerMap.value(plugin);

    foreach (MdiSubWindData* d, subWindMap)
    {
        delete d->_mdiSubWind;
        //delete d->_frameViewer;
        delete d;
    }
    _frameViewerMap.remove(plugin);
}

void CameraView::onPluginInitialised(NoobaPlugin *plugin)
{
    _paramConfigUI->addPlugin(plugin);
}

void CameraView::onPluginAboutToRelease(NoobaPlugin *plugin)
{
    _paramConfigUI->removePlugin(plugin);
}

void CameraView::onCreateFrameViewerRequest(const QString &title)
{
    NoobaPlugin* p = qobject_cast<NoobaPlugin*>(sender());  // get the sender of the signal
    if(!p)
        return;

    FrameViewer *fv = new FrameViewer(title, this);
    QMdiSubWindow* sw = addMDISubWindow(fv);
    sw->setVisible(true);
    MdiSubWindData* data = new MdiSubWindData(p, sw, fv);
    _frameViewerMap[p].insert(title, data);
    connect(p, SIGNAL(updateFrameViewer(QString,QImage)), this, SLOT(onFrameViewerUpdate(QString,QImage)));
}

void CameraView::onFrameViewerUpdate(const QString &title, const QImage &frame)
{
    NoobaPlugin* p = qobject_cast<NoobaPlugin*>(sender());  // get the sender of the signal
    if(!p)
        return;

    MdiSubWindData* d = _frameViewerMap.value(p).value(title);
    d->_frameViewer->updateFrame(frame);
}

void CameraView::onDebugMsg(const QString &debugMsg)
{

}

QImage CameraView::cvt2QImage(Mat &cvImg)
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
}
