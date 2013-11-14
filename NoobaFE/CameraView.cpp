#include "CameraView.h"
#include "ui_CameraView.h"
#include "SharedImageBuffer.h"
#include "CaptureThread.h"

// Qt
#include  <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>

CameraView::CameraView(SharedImageBuffer *sharedImageBuffer, QWidget *parent) :
    QWidget(parent),
    _isWebCam(false),
    ui(new Ui::CameraView),
    _sharedImageBuffer(sharedImageBuffer),
    _captureThread(NULL),
    _deviceNumber(-1),
    _imageBufferSize(1),
    _inputWind(tr("Video Input"))
{
    ui->setupUi(this);
    setWindowTitle(tr("Camera View"));
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

    _captureThread = new CaptureThread(_sharedImageBuffer, _deviceNumber,true);
    if(!_captureThread->connectToCamera())
    {
        QMessageBox errMsg;
        errMsg.setText(tr("Failed to open web-cam"));
        errMsg.setIcon(QMessageBox::Critical);
        errMsg.exec();
        delete _captureThread;
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
}

void CameraView::onPluginLoad(NoobaPlugin *plugin)
{

}

void CameraView::onPluginAboutToUnload(NoobaPlugin *plugin)
{

}

void CameraView::onPluginInitialised(NoobaPlugin *plugin)
{

}

void CameraView::onPluginAboutToRelease(NoobaPlugin *plugin)
{

}

void CameraView::onCreateFrameViewerRequest(const QString &title)
{

}

void CameraView::onFrameViewerUpdate(const QString &title, const QImage &frame)
{

}

QImage CameraView::cvt2QImage(Mat &cvImg)
{

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
