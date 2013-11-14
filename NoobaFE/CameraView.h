#ifndef CAMERAVIEW_H
#define CAMERAVIEW_H

#include "NoobaEye.h"
#include "FrameViewer.h"
#include "OutputWind.h"
#include "ParamConfigWind.h"

// Qt
#include <QWidget>
#include <QMap>
#include <QTimer>
#include <QScopedPointer>

namespace Ui {
class CameraView;
}
class SharedImageBuffer;
class CaptureThread;
class PluginLoader;
class NoobaPlugin;
class QMdiSubWindow;

class CameraView : public QWidget
{
    Q_OBJECT

public:

    explicit CameraView(SharedImageBuffer* sharedImageBuffer, QWidget *parent = 0);
    ~CameraView();

    ParamConfigWind* getParamConfigWind() { return _paramConfigUI.data(); }
    OutputWind* getDebugMsgWind() { return _debugOutWind.data(); }
    PluginLoader* getPluginLoader() { return _pluginLoader.data(); }

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

    void onDebugMsg(const QString& debugMsg);

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
    void stopCaptureThread();
    void connectSignalSlots();
    void initializeMdiArea();
    QMdiSubWindow *addMDISubWindow(FrameViewer* frameViewer);

    bool                            _isWebCam;
    Ui::CameraView                  *ui;
    SharedImageBuffer               *_sharedImageBuffer;
    QScopedPointer<CaptureThread>   _captureThread;
    QScopedPointer<PluginLoader>    _pluginLoader;

    int                             _deviceNumber;
    int                             _imageBufferSize;
    nooba::VideoState               _vidState;
    ProcParams                      _params;
    QTimer				            _timer;
    FrameViewer                     _inputWind;
    QScopedPointer<OutputWind>      _debugOutWind;
    QScopedPointer<ParamConfigWind> _paramConfigUI;
    QMap<NoobaPlugin*, QMap<QString, MdiSubWindData*> >  _frameViewerMap;
};

#endif // CAMERAVIEW_H
