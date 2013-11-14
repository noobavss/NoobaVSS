#ifndef CAMERAVIEW_H
#define CAMERAVIEW_H

#include "NoobaEye.h"
#include "PluginLoader.h"
#include "FrameViewer.h"

#include <QWidget>

namespace Ui {
class CameraView;
}
class SharedImageBuffer;
class CaptureThread;

class CameraView : public QWidget
{
    Q_OBJECT

public:

    explicit CameraView(SharedImageBuffer* sharedImageBuffer, QWidget *parent = 0);
    ~CameraView();

public slots:

    void connectToCamera();
    void connectToVideoFileStream();

private slots:

    void on_controlButton_clicked();

    void onPluginLoad(NoobaPlugin *plugin);
    void onPluginAboutToUnload(NoobaPlugin *plugin);
    void onPluginInitialised(NoobaPlugin* plugin);
    void onPluginAboutToRelease(NoobaPlugin* plugin);
    void onCreateFrameViewerRequest(const QString& title);
    void onFrameViewerUpdate(const QString& title, const QImage& frame);

private:

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
    void stopCaptureThread();

    bool                            _isWebCam;
    Ui::CameraView                  *ui;
    SharedImageBuffer               *_sharedImageBuffer;
    CaptureThread                   *_captureThread;

    int                             _deviceNumber;
    int                             _imageBufferSize;
    nooba::VideoState               _vidState;
    ProcParams                      _params;
    PluginLoader                    _pluginLoader;
    FrameViewer                     _inputWind;
};

#endif // CAMERAVIEW_H
