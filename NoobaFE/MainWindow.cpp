/**
    @author  Asitha Nanayakkara <daun07@gmail.com>

    @section LICENSE
    Nooba Plugin API source file
    Copyright (C) 2013 Developed by Team Nooba

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "PluginsConfigUI.h"
#include "NoobaPlugin.h"
#include "ParamConfigWind.h"
#include "CameraView.h"

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
#include <QInputDialog>

// opencv includes
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _sharedImageBuffer(new SharedImageBuffer())
{
	ui->setupUi(this);
    initMDIArea();
    ui->menu_Window->addAction(ui->paramConfigDock->toggleViewAction());
    ui->menu_Window->addAction(ui->debugOutputDock->toggleViewAction());
    ui->menu_Window->addAction(ui->statPanelDock->toggleViewAction());
    updateDockWidgets(&_paramConfigUI, &_debugWind, &_statPanel);
    setWindowTitle(nooba::ProgramName);
    enableMenuItems(false);
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
    if(!tabCheck())
        return;

    CameraView* camView = new CameraView(_sharedImageBuffer);
    if(camView->connectToVideoFileStream())
    {
        addNewSourceTab(camView);
        ui->statusBar->showMessage(tr("file stream opened to read frames"));
    }
    else
    {
        delete camView;
    }
}

void MainWindow::onOpenWebCam()
{
    if(!tabCheck())
        return;

    CameraView* camView = new CameraView(_sharedImageBuffer);
    bool ok;
    int deviceNumber = QInputDialog::getInt(this, tr("Device Number"),
                                      tr("Device number of the capture device <br>[web cam is usually 0]"),
                                      0, 0, 999,1, &ok);
    if(!ok)
    {
        delete camView;
        return;
    }

    if(camView->connectToCamera(deviceNumber))
    {
        addNewSourceTab(camView);
        ui->statusBar->showMessage(tr("Web cam is set as default input source."));
    }
    else
    {
        delete camView;
    }
}

//void MainWindow::onNextButton_clicked()
//{
////    updateFrame();
//}

//void MainWindow::onPrevButton_clicked()
//{
////    double cTime = _vidCapture.get(CV_CAP_PROP_POS_MSEC);
////    _vidCapture.set(CV_CAP_PROP_POS_MSEC, cTime - _delay * 2); // set previous frame to be read next
////    updateFrame();
//}

void MainWindow::onControlButton_clicked()
{
    CameraView *camView = getActiveCameraView();
    if(!camView)
        return;

    camView->on_controlButton_clicked();
}

void MainWindow::onMdiSubWindowActivated(QMdiSubWindow *subWindow)
{
    if(!subWindow)
    {
        updateDockWidgets(&_paramConfigUI, &_debugWind, &_statPanel);
        return;
    }

    CameraView* camView = qobject_cast<CameraView* >(subWindow->widget());
    if(!camView)
        return;

    updateDockWidgets(camView->getParamConfigWind(), camView->getDebugMsgWind(), camView->getStatPanel());
    return;
}

void MainWindow::onCamViewClose()
{
    if(ui->mdiArea->subWindowList().count() == 0) // 1 is goint to close. so disable menus
        enableMenuItems(false);
}

bool MainWindow::tabCheck()
{
    if( ui->mdiArea->subWindowList().count() == 1)
    {
        QMessageBox msg;
        msg.setText(tr("Multiple tabs are not supported in this version." \
                       " Close the current tab and open a new tab"));
        msg.exec();

        return false;
    }
    enableMenuItems(true);
    return true;
}

void MainWindow::enableMenuItems(bool isEnable)
{
    ui->actionNext_Frame->setEnabled(isEnable);
    ui->actionPlay_Pause->setEnabled(isEnable);
    ui->actionPrevious_Frame->setEnabled(isEnable);
    ui->actionPlugins->setEnabled(isEnable);

    return;
}

void MainWindow::updateDockWidgets(ParamConfigWind* paramConfig, OutputWind* debugMsgWind, StatPanel* statPanel)
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->paramConfigDock->widget()->layout());
    if(layout)
    {
        QWidget *w = layout->itemAt(0)->widget();
        if(w)
            w->setParent(0);
        delete layout;
    }
    layout = new QVBoxLayout;
    layout->addWidget(paramConfig);
    layout->setContentsMargins(0,0,0,0);
    ui->paramConfigDock->widget()->setLayout(layout);

    layout = qobject_cast<QVBoxLayout*>(ui->debugOutputDock->widget()->layout());
    if(layout)
    {
        QWidget* w = layout->itemAt(0)->widget();
        if(w)
            w->setParent(0);
        delete layout;
    }
    layout = new QVBoxLayout;
    layout->addWidget(debugMsgWind);
    layout->setContentsMargins(0,0,0,0);
    ui->debugOutputDock->widget()->setLayout(layout);

    layout = qobject_cast<QVBoxLayout*>(ui->statPanelDock->widget()->layout());
    if(layout)
    {
        QWidget* w = layout->itemAt(0)->widget();
        if(w)
            w->setParent(0);
        delete layout;
    }
    layout = new QVBoxLayout;
    layout->addWidget(statPanel);
    layout->setContentsMargins(0,0,0,0);
    ui->statPanelDock->widget()->setLayout(layout);

}

QMdiSubWindow* MainWindow::addNewSourceTab(CameraView* camView)
{
    QMdiSubWindow* subWind = ui->mdiArea->addSubWindow(camView);
    subWind->showMaximized();
    ui->mdiArea->setActiveSubWindow(subWind);
    connect(subWind, SIGNAL(destroyed()), this, SLOT(onCamViewClose()));
    return subWind;
}

void MainWindow::initMDIArea()
{
    // adjust tab bar width
    QList<QTabBar *> tabBarList = ui->mdiArea->findChildren<QTabBar*>();
    QTabBar *tabBar = tabBarList.first();
    if (tabBar) {
        tabBar->setExpanding(false);
    }

    // listen to subwindow changes
    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(onMdiSubWindowActivated(QMdiSubWindow*)));
}

CameraView *MainWindow::getActiveCameraView()
{
    QMdiSubWindow *subWindow = ui->mdiArea->activeSubWindow();
    if(!subWindow)
        return NULL;

    CameraView  *camView = qobject_cast<CameraView*>(subWindow->widget());
    return camView;
}

void MainWindow::onPluginAct_triggerred()
{
    CameraView  *camView = getActiveCameraView();
    if(!camView)
        return;
    camView->configurePlugins();
}

void MainWindow::on_actionAbout_NoobaVSS_triggered()
{
    QString arch;
#if defined(Q_PROCESSOR_X86_32)
    arch = "32 bit";
#elif defined(Q_PROCESSOR_X86_64)
    arch = "64 bit";
#endif

    QMessageBox::about(this, "About NoobaVSS",
        QString("<h2 style=\"font-family:sans-serif;\" >NoobaVSS %1.%2 (%3)</h2>"
                "<p>Nooba Plugin API version %4.%5</p>"
                "<p>Using OpenCV %6.%7 and Qt %8</p>"
                "<p>Built on %9 at %10</p>"
                "<p>Copyright (C) 2014 by the Nooba team. All rights reserved.</p>"
                "<p>For more info visit us on <a href=\"https://noobavss.github.io/\" >https://noobavss.github.io/</a></p>"
                "<p><b>NoobaVSS</b> is free software: you can redistribute it and/or modify "
                "it under the terms of the GNU General Public License as published by "
                "the Free Software Foundation, either version 3 of the License, or "
                "(at your option) any later version.</p>"
                "<p>This program is distributed in the hope that it will be useful,"
                " but <b>WITHOUT ANY WARRANTY;</b> without even the implied warranty of"
                " <b>MERCHANTABILITY</b> or <b>FITNESS FOR A PARTICULAR PURPOSE.</b></p>")
                       .arg(nooba::MajorVersion).arg(nooba::MinorVersion)
                       .arg(arch)
                       .arg(API_MAJOR_VERSION).arg(API_MINOR_VERSION)
                       .arg(CV_MAJOR_VERSION).arg(CV_MINOR_VERSION)
                       .arg(QT_VERSION_STR)
                       .arg(__DATE__).arg(__TIME__)
                       );
}
