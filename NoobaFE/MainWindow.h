#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// project files
#include "NoobaEye.h"
#include "PluginLoader.h"

// Qt includes
#include <QMainWindow>
#include <QScopedPointer>
#include <QTimer>

// Opencv includes
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

// forward declarations
class FrameProcessor;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:

    void onOpenFile();
	void onOpenWebCam();	
    void on_nextButton_clicked();
    void on_prevButton_clicked();
	void on_playButton_clicked();
	void updateFrame();
    void onPluginAct_triggerred();
    
private:

	/*
	 \brief get a color QImage from cv::Mat
	 \return QImage with three color channels 
	 */
    inline QImage convertToQImage(cv::Mat &cvImg);

	/* 
	 \brief get a grayscale QImage from cv::Mat
	 \return QImage returns single color channel QImage
	 */
	inline QImage grayQImage(cv::Mat& cvImg);

	/* Enum to define the current state of the video
	 */
	enum VideoState {
        PlayingState    = 0,
        PausedState     = 1,
        StoppedState    = 2
	};

	/*
	 *	Updates the current video state
	 *  \param state 
	 */
	void setVideoState(VideoState state);
	

    Ui::MainWindow                  *ui;
    QScopedPointer<FrameProcessor>  _FrameProc;

    cv::VideoCapture                _vidCapture;
    cv::Mat                         _frame;
    QTimer				            _timer;
    VideoState			            _vidState;
    ProcParams                      _params;
    PluginLoader                    _pluginLoader;
    int                             _delay;

};

#endif // MAINWINDOW_H
