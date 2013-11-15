#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// project files
#include "NoobaEye.h"
#include "SharedImageBuffer.h"
#include "CameraView.h"
#include "ParamConfigWind.h"
#include "OutputWind.h"

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
    void on_nextButton_clicked();
    void on_prevButton_clicked();
	void updateFrame();
    void onPluginAct_triggerred();
    
    void on_actionAbout_NoobaVSS_triggered();
    void on_controlButton_clicked();
    void onMdiSubWindowActivated(QMdiSubWindow* subWindow);

private:

    void updateDockWidgets(ParamConfigWind *paramConfig, OutputWind *debugMsgWind);
    CameraView *addNewSourceTab();
    void initMDIArea();

    Ui::MainWindow                  *ui;
    SharedImageBuffer               *_sharedImageBuffer;
    ParamConfigWind                 _paramConfigUI;
    OutputWind                      _debugWind;

};

#endif // MAINWINDOW_H
