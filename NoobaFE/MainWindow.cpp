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
#include <QMapIterator>
#include <CameraView.h>

// opencv includes
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _sharedImageBuffer(new SharedImageBuffer())
{
	ui->setupUi(this);
//    connect(&_timer, SIGNAL(timeout()), this, SLOT(updateFrame()));


//    updateDockWidgets();
//    connectSignalSlots();

//    initMDIArea();
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

//    _vidCapture.release();  // release any currently loaded capture device if any
//    setVideoState(nooba::StoppedState);

//	// load new video capture device
//    if(!_vidCapture.open(path.toStdString()))
//    {
//        QMessageBox errMsg;
//        errMsg.setText(tr("File open failed from path \"%1\"").arg(path));
//        errMsg.exec();
//        return;
//    }
//    _isWebCam = false;
////    ui->prevButton->setEnabled(true);   // enable the previous button on this mode.
//    _params.setFrameId(0);

//    if(!_firstRun)
//        _pluginLoader.refreshPlugins();

//    _firstRun = false;
//    ui->statusBar->showMessage(tr("file opened: %1").arg(path), 6000);
}

void MainWindow::onOpenWebCam()
{
    CameraView* camView = addNewSourceTab();
    camView->connectToCamera();
//    ui->statusBar->showMessage(tr("Web cam is set as default input source."));
}

void MainWindow::on_nextButton_clicked()
{
//    updateFrame();
}

void MainWindow::on_prevButton_clicked()
{
//    double cTime = _vidCapture.get(CV_CAP_PROP_POS_MSEC);
//    _vidCapture.set(CV_CAP_PROP_POS_MSEC, cTime - _delay * 2); // set previous frame to be read next
//    updateFrame();
}

void MainWindow::on_controlButton_clicked()
{
}

void MainWindow::updateFrame()
{
//    if(!_pluginLoader.getBasePlugin()) //  if active plugin not set
//    {
//        setVideoState(nooba::StoppedState);
//        QMessageBox msgBox;
//        msgBox.setText(tr("No video processing plugin is set. Set a plugin before playing the video stream."));
//        msgBox.setIcon(QMessageBox::Warning);
//        msgBox.exec();
//        onPluginAct_triggerred(); // pop plugin config window
//    }

//    if (!_vidCapture.read(_frame))
//	{
//        setVideoState(nooba::StoppedState);
//        _vidCapture.set(CV_CAP_PROP_POS_FRAMES, 0);
//		return;
//	}

//    if(!_isWebCam)
//        _params.setFrameId(_vidCapture.get(CV_CAP_PROP_POS_FRAMES) - 1);

//    cv::cvtColor(_frame, _frame, CV_BGR2RGB); // convert layout from BGR to RGB.
//    _inputWind.updateFrame(cvt2QImage(_frame));
//	cv::Mat editedFrame;

//    if(_pluginLoader.getBasePlugin()->procFrame(_frame, editedFrame, _params))
//	{
//        _outputWind.updateFrame(cvt2QImage(editedFrame));
//	}
}

void MainWindow::updateDockWidgets(CameraView* camView)
{
    //    QVBoxLayout *layout = new QVBoxLayout;
    //    layout->addWidget(_paramConfigUI);
    //    layout->setContentsMargins(0,0,0,0);
    //    ui->paramConfigDock->widget()->setLayout(layout);

    //    layout = new QVBoxLayout;
    //    layout->addWidget(&_dbugOutWind);
    //    layout->setContentsMargins(0,0,0,0);
    //    ui->debugOutputDock->widget()->setLayout(layout);

    //    if(_pluginLoader.getBasePlugin())
    //    {
    //       onPluginLoad(_pluginLoader.getBasePlugin());
    //    }
}

CameraView* MainWindow::addNewSourceTab()
{
    CameraView* camView = new CameraView(_sharedImageBuffer);
    QMdiSubWindow* subWind = ui->mdiArea->addSubWindow(camView);
    subWind->showMaximized();
    return camView;
}

void MainWindow::initMDIArea()
{
}


void MainWindow::onPluginAct_triggerred()
{
    QMdiSubWindow *subWindow = ui->mdiArea->activeSubWindow();
    if(!subWindow)
        return;

    CameraView  *camView = qobject_cast<CameraView*>(subWindow->widget());
    if(!camView)
        return;

    PluginsConfigUI dlg(*camView->getPluginLoader(), this);
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
