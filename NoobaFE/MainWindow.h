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
	void updateFrame();
    void onPluginAct_triggerred();
    
    void on_actionAbout_NoobaVSS_triggered();
    void onControlButton_clicked();
    void onMdiSubWindowActivated(QMdiSubWindow* subWindow);

private:

    void updateDockWidgets(ParamConfigWind *paramConfig, OutputWind *debugMsgWind, StatPanel* statPanel);
    CameraView *addNewSourceTab();
    void initMDIArea();
    CameraView *getActiveCameraView();

    Ui::MainWindow                  *ui;
    SharedImageBuffer               *_sharedImageBuffer;
    ParamConfigWind                 _paramConfigUI;
    OutputWind                      _debugWind;
    StatPanel                       _statPanel;

};

#endif // MAINWINDOW_H
