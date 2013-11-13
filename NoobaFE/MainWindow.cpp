#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "PluginsConfigUI.h"
#include "NoobaPlugin.h"
#include "ParamConfigWind.h"

// Qt includes
#if QT_VERSION >= 0x050000
    #include <QStandardPaths>
#else
    #include <QDesktopServices>
#endif
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QDateTime>

// opencv includes
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _paramConfigUI(new ParamConfigWind(this)),
    _delay(0),
    _isWebCam(false),
    _firstRun(true),
    _vidState(nooba::StoppedState),
    _inputWind("Input", this),
    _outputWind("output", this)
{
	ui->setupUi(this);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(updateFrame()));
    updateDockWidgets();
    connectSignalSlots();
    _pluginLoader.loadPrevConfig();
    _pluginLoader.loadPluginInfo();
    initMDIArea();
    setWindowTitle(nooba::ProgramName);
}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
// TODO: Give user the option to choose whether to get the warning on close or not.
//       this bothers some users

//    int ret  = QMessageBox::warning(this, tr("Close Request"),
//              tr("Are you sure you want to close the application?"), QMessageBox::Yes | QMessageBox::No);

//    if(ret == QMessageBox::No)
//        event->ignore();

}

void MainWindow::onOpenFile()
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

    _vidCapture.release();  // release any currently loaded capture device if any
    setVideoState(nooba::StoppedState);

	// load new video capture device
    if(!_vidCapture.open(path.toStdString()))
    {
        QMessageBox errMsg;
        errMsg.setText(tr("File open failed from path \"%1\"").arg(path));
        errMsg.exec();
        return;
    }
    _isWebCam = false;
    ui->prevButton->setEnabled(true);   // enable the previous button on this mode.
    _params.setFrameId(0);

    if(!_firstRun)
        _pluginLoader.refreshPlugins();

    _firstRun = false;
    ui->statusBar->showMessage(tr("file opened: %1").arg(path), 6000);
}

void MainWindow::onOpenWebCam()
{
    _vidCapture.release();
    setVideoState(nooba::StoppedState);

    if(!_vidCapture.open(0))
	{
		QMessageBox errMsg;
		errMsg.setText(tr("Failed to open web-cam"));
        errMsg.setIcon(QMessageBox::Critical);
		errMsg.exec();
		return;
	}
    _isWebCam = true;
    ui->prevButton->setDisabled(true);  // disable on web cam mode, irrelavant
    _params.setFrameId(-1);

    if(!_firstRun)
        _pluginLoader.refreshPlugins();

    _firstRun = false;
    ui->statusBar->showMessage(tr("Web cam is set as default input source."));
}

QImage MainWindow::cvt2QImage(cv::Mat& cvImg)
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

void MainWindow::on_nextButton_clicked()
{
    updateFrame();	
}

void MainWindow::on_prevButton_clicked()
{
    double cTime = _vidCapture.get(CV_CAP_PROP_POS_MSEC);
    _vidCapture.set(CV_CAP_PROP_POS_MSEC, cTime - _delay * 2); // set previous frame to be read next
    updateFrame();
}

void MainWindow::on_controlButton_clicked()
{
    if(_vidState == nooba::PlayingState)
    {
        setVideoState(nooba::PausedState);
        return;
    }

    double rate= _vidCapture.get(CV_CAP_PROP_FPS);

    if(rate > 0)    // video file
    {
        _delay = 1000/rate;
        _params.setFrameRate(rate);
    }
    else    // on web cam open
    {
        _delay = 50;   // assume 10 fps
        _params.setFrameRate(20);
    }

    _timer.start(_delay);
    setVideoState(nooba::PlayingState);
}

void MainWindow::updateFrame()
{
    if(!_pluginLoader.getBasePlugin()) //  if active plugin not set
    {
        setVideoState(nooba::StoppedState);
        QMessageBox msgBox;
        msgBox.setText(tr("No video processing plugin is set. Set a plugin before playing the video stream."));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        onPluginAct_triggerred(); // pop plugin config window
    }

    if (!_vidCapture.read(_frame))
	{
        setVideoState(nooba::StoppedState);
        _vidCapture.set(CV_CAP_PROP_POS_FRAMES, 0);
		return;
	}

    if(!_isWebCam)
        _params.setFrameId(_vidCapture.get(CV_CAP_PROP_POS_FRAMES) - 1);

    cv::cvtColor(_frame, _frame, CV_BGR2RGB); // convert layout from BGR to RGB.
    _inputWind.updateFrame(cvt2QImage(_frame));
	cv::Mat editedFrame;

    if(_pluginLoader.getBasePlugin()->procFrame(_frame, editedFrame, _params))
	{
        _outputWind.updateFrame(cvt2QImage(editedFrame));
	}
}

void MainWindow::setVideoState( nooba::VideoState state )
{
	switch(state){

    case nooba::StoppedState:
		{
            _vidState = nooba::StoppedState;
            _timer.stop();
            ui->controlButton->setIcon(QIcon(":/Resources/super-mono-iconset/button-play.png"));
            ui->controlButton->setToolTip(tr("Play"));
            break;
		}
    case nooba::PausedState:
		{
            _vidState = nooba::PausedState;
            ui->controlButton->setIcon(QIcon(":/Resources/super-mono-iconset/button-play.png"));
            ui->controlButton->setToolTip(tr("Play"));
            _timer.stop();
            break;
		}
    case nooba::PlayingState:
		{
            _vidState = nooba::PlayingState;
            ui->controlButton->setIcon(QIcon(":/Resources/super-mono-iconset/button-pause.png"));
            ui->controlButton->setToolTip(tr("Pause"));
            break;
		}
	default:
        break;
    }
    _params.setVidState(_vidState);
}

void MainWindow::updateDockWidgets()
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(_paramConfigUI);
    layout->setContentsMargins(0,0,0,0);
    ui->paramConfigDock->widget()->setLayout(layout);

    layout = new QVBoxLayout;
    layout->addWidget(&_dbugOutWind);
    layout->setContentsMargins(0,0,0,0);
    ui->debugOutputDock->widget()->setLayout(layout);

    if(_pluginLoader.getBasePlugin())
    {
       onPluginLoad(_pluginLoader.getBasePlugin());
    }
}

void MainWindow::connectSignalSlots()
{
    connect(&_pluginLoader, SIGNAL(pluginLoaded(NoobaPlugin*)), this, SLOT(onPluginLoad(NoobaPlugin*)));
    connect(&_pluginLoader, SIGNAL(pluginAboutToUnloaded(NoobaPlugin*)), this, SLOT(onPluginAboutToRelease(NoobaPlugin*)));
    connect(&_pluginLoader, SIGNAL(pluginInitialised(NoobaPlugin*)), this, SLOT(onPluginInitialised(NoobaPlugin*)));
    connect(&_pluginLoader, SIGNAL(pluginAboutToRelease(NoobaPlugin*)), this, SLOT(onPluginAboutToRelease(NoobaPlugin*)));
}

void MainWindow::initMDIArea()
{
    ui->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    addMDISubWindow(&_inputWind);
    addMDISubWindow(&_outputWind);
    ui->mdiArea->tileSubWindows();
}

QMdiSubWindow* MainWindow::addMDISubWindow(FrameViewer *frameViewer)
{
    QMdiSubWindow *mdiWind = ui->mdiArea->addSubWindow(frameViewer);
    mdiWind->setContentsMargins(0,0,0,0);
    return mdiWind;
}

void MainWindow::onPluginAct_triggerred()
{
   PluginsConfigUI dlg(_pluginLoader, this);
   dlg.exec();
}

void MainWindow::on_actionAbout_NoobaVSS_triggered()
{

    QMessageBox::about(this, "Nooba VSS",
                       QString("<b>Nooba is an open source surveillance video analysis tool</b><ul><li>API Version:\t\t\t")
                       .append(QString::number(API_MAJOR_VERSION)).append(".").append(QString::number(API_MINOR_VERSION))
                       .append("</li><li>OpenCV version:\t\t\t").append(QString::number(CV_MAJOR_VERSION))
                       .append(".").append(QString::number(CV_MINOR_VERSION))
                       .append("</li><li>App version:\t\t\t").append(QString("").number(nooba::MajorVersion))
                       .append(".").append(QString::number(nooba::MinorVersion)).append("</li><li>")
                       .append("Qt version:\t\t\t").append(QT_VERSION_STR).append("</li></ul>")
                       .append("<p>Build on %1</p>").arg(QDateTime::currentDateTime().toString())
                       );
}

void MainWindow::on_TileviewButton_clicked()
{
    ui->mdiArea->tileSubWindows();
}

void MainWindow::onPluginLoad(NoobaPlugin *plugin)
{    
    connect(plugin, SIGNAL(debugMsg(QString)), &_dbugOutWind, SLOT(onDebugMsg(QString)));
    connect(plugin, SIGNAL(createFrameViewer(QString)), this, SLOT(onCreateFrameViewerRequest(QString)));
    _frameViewerMap.insert(plugin, QMap<QString, MdiSubWindData* >());
}

void MainWindow::onPluginAboutUnload(NoobaPlugin* plugin)
{
    QMap<NoobaPlugin*, QMap<QString, MdiSubWindData*> >::const_iterator i = _frameViewerMap.find(plugin);
    if(i == _frameViewerMap.end())
        return;

    foreach (MdiSubWindData* d, i.value())
    {
        delete d;
    }
    _frameViewerMap.remove(plugin);
}

void MainWindow::onPluginInitialised(NoobaPlugin *plugin)
{
    _paramConfigUI->addPlugin(plugin);
}

void MainWindow::onPluginAboutToRelease(NoobaPlugin *plugin)
{
    _paramConfigUI->removePlugin(plugin);
}

void MainWindow::onCreateFrameViewerRequest(const QString &title)
{
    NoobaPlugin* p = qobject_cast<NoobaPlugin*>(sender());  // get the sender of the signal
    if(!p)
        return;

    FrameViewer *fv = new FrameViewer(title, this);
    QMdiSubWindow* sw = addMDISubWindow(fv);
    MdiSubWindData* data = new MdiSubWindData(p, sw, fv);
    _frameViewerMap[p].insert(title, data);
    connect(p, SIGNAL(updateFrameViewer(QString,QImage)), this, SLOT(onFrameViewerUpdate(QString,QImage)));
}

void MainWindow::onFrameViewerUpdate(const QString &title, const QImage &frame)
{
    NoobaPlugin* p = qobject_cast<NoobaPlugin*>(sender());  // get the sender of the signal
    if(!p)
        return;

    MdiSubWindData* d = _frameViewerMap.value(p).value(title);
    d->_frameViewer->updateFrame(frame);
}

