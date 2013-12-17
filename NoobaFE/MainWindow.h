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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// project files
#include "NoobaEye.h"
#include "SharedImageBuffer.h"
#include "CameraView.h"
#include "ParamConfigWind.h"
#include "OutputWind.h"
#include "StatPanel.h"

// Qt includes
#include <QMainWindow>
#include <QScopedPointer>
#include <QMdiSubWindow>

namespace Ui {
    class MainWindow;
}

namespace nooba {
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

    void closeEvent(QCloseEvent *event);

private slots:

    void onOpenFile();
	void onOpenWebCam();	
//    void onNextButton_clicked();
//    void onPrevButton_clicked();
    void onPluginAct_triggerred();
    
    void on_actionAbout_NoobaVSS_triggered();
    void onControlButton_clicked();
    void onMdiSubWindowActivated(QMdiSubWindow* subWindow);
    void onCamViewClose();

private:

    bool tabCheck();
    void enableMenuItems(bool isEnable);

    void updateDockWidgets(ParamConfigWind *paramConfig, OutputWind *debugMsgWind, StatPanel* statPanel);
    QMdiSubWindow *addNewSourceTab(CameraView *camView);
    void initMDIArea();
    CameraView *getActiveCameraView();

    Ui::MainWindow                  *ui;
    SharedImageBuffer               *_sharedImageBuffer;
    ParamConfigWind                 _paramConfigUI;
    OutputWind                      _debugWind;
    StatPanel                       _statPanel;

};

#endif // MAINWINDOW_H
