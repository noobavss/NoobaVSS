#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "FrameProcessor.h"
#include "PluginsConfigUI.h"

// Qt includes
#include <QDebug>
#include <QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>

// opencv includes
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _FrameProc(new FrameProcessor(parent)),
    _vidState(StoppedState)
{
	ui->setupUi(this);
	// align the labels to center
	ui->orgImg->setAlignment(Qt::AlignCenter);
	ui->editedImg->setAlignment(Qt::AlignCenter);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(updateFrame()));
    _pluginLoader.loadPluginInfo();
}

MainWindow::~MainWindow()
{
    delete ui;
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
}

void MainWindow::onOpenWebCam()
{
    _vidCapture.release();
	setVideoState(StoppedState);

    if(!_vidCapture.open(0))
	{
		QMessageBox errMsg;
		errMsg.setText(tr("Failed to open web-cam"));
		errMsg.exec();
		return;
	}
    _params.setFrameId(0);
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


    ui->orgImg->setPixmap(QPixmap::fromImage(
        convertToQImage(_frame))
		);
	cv::Mat editedFrame;

    ;
    if(_pluginLoader.getActivePlugin()->procFrame(_frame, editedFrame, _params))
	{
		ui->editedImg->setPixmap(
			QPixmap::fromImage(grayQImage(editedFrame))
			);
	}
}

void MainWindow::setVideoState( VideoState state )
{
	switch(state){

	case StoppedState:
		{
            _vidState = StoppedState;
            _timer.stop();
			ui->controlButton->setText(tr("Play"));
			return;
		}
	case PausedState:
		{
            _vidState = PausedState;
			ui->controlButton->setText(tr("Play"));
            _timer.stop();
			return;
		}
	case PlayingState:
		{
            _vidState = PlayingState;
			ui->controlButton->setText(tr("Pause"));
			return;
		}
	default:
		return;
	}
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
