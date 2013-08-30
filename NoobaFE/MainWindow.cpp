#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "PluginsConfigUI.h"
#include "ParamConfigWind.h"

// Qt includes
#include <QDebug>
#include <QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QCloseEvent>
#include <QVBoxLayout>

// opencv includes
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _paramConfigUI(new ParamConfigWind(this)),
    _delay(0),
    _vidState(StoppedState),
    _inputWind("Input", this),
    _outputWind("output", this)
{
	ui->setupUi(this);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(updateFrame()));
    updateDockWidgets();

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
    int ret  = QMessageBox::warning(this, tr("Close Request"),
              tr("Are you sure you want to close the application?"), QMessageBox::Yes | QMessageBox::No);

    if(ret == QMessageBox::No)
        event->ignore();

}

void MainWindow::onOpenFile()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open file"),
                      QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));

    if(path.isEmpty())
        return;

    _vidCapture.release();  // release any currently loaded capture device if any
    setVideoState(StoppedState);

	// load new video capture device
    if(!_vidCapture.open(path.toStdString()))
    {
        QMessageBox errMsg;
        errMsg.setText(tr("File open failed from path \"%1\"").arg(path));
        errMsg.exec();
        return;
    }
    _params.setFrameId(0);
    ui->statusBar->showMessage(tr("file opened: %1").arg(path), 6000);
}

void MainWindow::onOpenWebCam()
{
    _vidCapture.release();
	setVideoState(StoppedState);

    if(!_vidCapture.open(0))
	{
		QMessageBox errMsg;
		errMsg.setText(tr("Failed to open web-cam"));
        errMsg.setIcon(QMessageBox::Critical);
		errMsg.exec();
		return;
	}
    _params.setFrameId(0);
    ui->statusBar->showMessage(tr("Web cam is set as default input source."));
}

QImage MainWindow::convertToQImage(cv::Mat& cvImg)
{
    return QImage((const unsigned char*)(cvImg.data),
                cvImg.cols,cvImg.rows,cvImg.step,  QImage::Format_RGB888);
}

QImage MainWindow::grayQImage( cv::Mat& cvImg )
{
	return QImage((const unsigned char*)(cvImg.data),
		cvImg.cols,cvImg.rows,cvImg.step,  QImage::Format_Indexed8);
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
    if(_vidState == PlayingState)
    {
        setVideoState(PausedState);
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
        _delay = 100;   // assume 10 fps
        _params.setFrameRate(10);
    }

    _timer.start(_delay);
    setVideoState(PlayingState);
}

void MainWindow::updateFrame()
{
    if(!_pluginLoader.getActivePlugin()) //  if active plugin not set
    {
        setVideoState(StoppedState);
        QMessageBox msgBox;
        msgBox.setText(tr("No video processing plugin is set. Set a plugin before playing the video stream."));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        onPluginAct_triggerred(); // pop plugin config window
    }

    if (!_vidCapture.read(_frame))
	{
		setVideoState(StoppedState);
		return;
	}

    cv::cvtColor(_frame, _frame,CV_BGR2RGB); // convert layout from BGR to RGB
    _params.setFrameId(_vidCapture.get(CV_CAP_PROP_POS_FRAMES) - 1);

    _inputWind.updateFrame(convertToQImage(_frame));

	cv::Mat editedFrame;

    ;
    if(_pluginLoader.getActivePlugin()->procFrame(_frame, editedFrame, _params))
	{
        _outputWind.updateFrame(grayQImage(editedFrame));
	}
}

void MainWindow::setVideoState( VideoState state )
{
	switch(state){

	case StoppedState:
		{
            _vidState = StoppedState;
            _timer.stop();
            ui->controlButton->setIcon(QIcon(":/Resources/super-mono-iconset/button-play.png"));
            ui->controlButton->setToolTip(tr("Play"));
			return;
		}
	case PausedState:
		{
            _vidState = PausedState;
            ui->controlButton->setIcon(QIcon(":/Resources/super-mono-iconset/button-play.png"));
            ui->controlButton->setToolTip(tr("Play"));
            _timer.stop();
			return;
		}
	case PlayingState:
		{
            _vidState = PlayingState;
            ui->controlButton->setIcon(QIcon(":/Resources/super-mono-iconset/button-pause.png"));
            ui->controlButton->setToolTip(tr("Pause"));
			return;
		}
	default:
		return;
    }
}

void MainWindow::updateDockWidgets()
{
    connect(&_pluginLoader, SIGNAL(pluginLoaded(NoobaPlugin*)), this, SLOT(onPluginLoad(NoobaPlugin*)));
    connect(&_pluginLoader, SIGNAL(pluginUnloaded()), this, SLOT(onPluginUnload()));


    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(_paramConfigUI);
    layout->setContentsMargins(0,0,0,0);
    ui->paramConfigDock->widget()->setLayout(layout);

    layout = new QVBoxLayout;
    layout->addWidget(&_dbugOutWind);
    layout->setContentsMargins(0,0,0,0);
    ui->debugOutputDock->widget()->setLayout(layout);

    if(_pluginLoader.getActivePlugin())
    {
       onPluginLoad(_pluginLoader.getActivePlugin());

    }
}

void MainWindow::initMDIArea()
{
    ui->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QMdiSubWindow *sw = ui->mdiArea->addSubWindow(&_outputWind);
    sw->setContentsMargins(0,0,0,0);
    sw = ui->mdiArea->addSubWindow(&_inputWind);
    sw->setContentsMargins(0,0,0,0);
    ui->mdiArea->tileSubWindows();
}

void MainWindow::onPluginAct_triggerred()
{
   PluginsConfigUI dlg(_pluginLoader, this);
   dlg.exec();
}

void MainWindow::on_actionAbout_NoobaVSS_triggered()
{
    QMessageBox::about(this, "Nooba VSS",
                       QString("<h5>Nooba is an open source surveillance video analysis tool</h5><p>API Version:\t\t\t")
                       .append(QString::number(API_MAJOR_VERSION)).append(".").append(QString::number(API_MINOR_VERSION))
                       .append("</p><p>OpenCV version:\t\t\t").append(QString::number(CV_MAJOR_VERSION))
                       .append(".").append(QString::number(CV_MINOR_VERSION))
                       .append("</p><p>Version:\t\t\t").append(QString("").number(nooba::MajorVersion))
                       .append(".").append(QString::number(nooba::MinorVersion)).append("</p>")
                       );
}

void MainWindow::on_TileviewButton_clicked()
{
    ui->mdiArea->tileSubWindows();
}

void MainWindow::onPluginLoad(NoobaPlugin *plugin)
{
    _paramConfigUI->setPlugin(plugin);
    connect(plugin, SIGNAL(debugMsg(QString)), &_dbugOutWind, SLOT(onDebugMsg(QString)));
}

void MainWindow::onPluginUnload()
{
    _paramConfigUI->clear();
}

