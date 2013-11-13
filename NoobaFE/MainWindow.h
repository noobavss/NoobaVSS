#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// project files
#include "NoobaEye.h"
#include "PluginLoader.h"
#include "FrameViewer.h"
#include "OutputWind.h"

// Qt includes
#include <QMainWindow>
#include <QScopedPointer>
#include <QMdiSubWindow>
#include <QMap>
#include <QTimer>

// Opencv includes
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

// forward declarations
class ParamConfigWind;

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

    void on_TileviewButton_clicked();

    void onPluginLoad(NoobaPlugin *plugin);
    void onPluginAboutToUnload(NoobaPlugin *plugin);
    void onPluginInitialised(NoobaPlugin* plugin);
    void onPluginAboutToRelease(NoobaPlugin* plugin);
    void onCreateFrameViewerRequest(const QString& title);
    void onFrameViewerUpdate(const QString& title, const QImage& frame);

private:

    struct MdiSubWindData
    {
        MdiSubWindData(NoobaPlugin* plugin, QMdiSubWindow* subWind, FrameViewer* frameViewer)
            :_plugin(plugin), _mdiSubWind(subWind), _frameViewer(frameViewer){}

        NoobaPlugin*    _plugin;
        QMdiSubWindow*  _mdiSubWind;
        FrameViewer*    _frameViewer;
    };

	/*
	 \brief get a color QImage from cv::Mat
	 \return QImage with three color channels 
	 */
    inline QImage cvt2QImage(cv::Mat &cvImg);

    /*
	 *	Updates the current video state
	 *  \param state 
	 */
    void setVideoState(nooba::VideoState state);
    void updateDockWidgets();
    void connectSignalSlots();

    void initMDIArea();
    QMdiSubWindow *addMDISubWindow(FrameViewer* frameViewer);
	

    Ui::MainWindow                  *ui;
    ParamConfigWind                 *_paramConfigUI;
    int                             _delay;
    bool                            _isWebCam;
    bool                            _firstRun;
    cv::VideoCapture                _vidCapture;
    cv::Mat                         _frame;
    QTimer				            _timer;
    nooba::VideoState               _vidState;
    ProcParams                      _params;
    PluginLoader                    _pluginLoader;
    FrameViewer                     _inputWind;
    FrameViewer                     _outputWind;
    OutputWind                      _dbugOutWind;
    QMap<NoobaPlugin*, QMap<QString, MdiSubWindData*> >  _frameViewerMap;
};

#endif // MAINWINDOW_H
