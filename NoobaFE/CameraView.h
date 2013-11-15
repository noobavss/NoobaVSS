#ifndef CAMERAVIEW_H
#define CAMERAVIEW_H

#include "NoobaEye.h"
#include "FrameViewer.h"
#include "Structures.h"

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
class OutputWind;
class ParamConfigWind;
class StatPanel;
class ProcessingThread;
class PluginLoader;
class NoobaPlugin;
class QMdiSubWindow;

class CameraView : public QWidget
{
    Q_OBJECT

public:

    explicit CameraView(SharedImageBuffer* sharedImageBuffer, QWidget *parent = 0);
    ~CameraView();

    ParamConfigWind* getParamConfigWind() { return _paramConfigUI; }
    OutputWind* getDebugMsgWind() { return _debugOutWind; }
    StatPanel* getStatPanel() { return _statPanel; }
    PluginLoader* getPluginLoader() { return _pluginLoader.data(); }

public slots:

    void connectToCamera();
    void connectToVideoFileStream();
    void on_controlButton_clicked();

private slots:

    void onInputFrameUpdate(const QImage &in);

    void onPluginLoad(NoobaPlugin *plugin);
    void onPluginAboutToUnload(NoobaPlugin *plugin);
    void onPluginInitialised(NoobaPlugin* plugin);
    void onPluginAboutToRelease(NoobaPlugin* plugin);
    void onCreateFrameViewerRequest(const QString& title);
    void onFrameViewerUpdate(const QString& title, const QImage& frame);
    void updateCaptureThreadStats(struct ThreadStatisticsData statData);
    void updateProcessingThreadStats(struct ThreadStatisticsData statData);

private:

    struct MdiSubWindData
    {
        MdiSubWindData(NoobaPlugin* plugin, QMdiSubWindow* subWind, FrameViewer* frameViewer)
            :_plugin(plugin), _mdiSubWind(subWind), _frameViewer(frameViewer) {}

        NoobaPlugin*    _plugin;
        QMdiSubWindow*  _mdiSubWind;
        FrameViewer*    _frameViewer;
    };

    /*
     \brief get a color QImage from cv::Mat
     \return QImage with three color channels
     */
    inline QImage cvt2QImage(const cv::Mat &cvImg);

    /*
     *	Updates the current video state
     *  \param state
     */
    void setVideoState(nooba::VideoState state);
    void stopCaptureThread();
    void stopProcessingThread();
    void connectSignalSlots();
    void initializeMdiArea();
    inline void setupSharedBufferForNewDevice();
    QMdiSubWindow *addMDISubWindow(FrameViewer* frameViewer);

    bool                                _isWebCam;
    Ui::CameraView                      *ui;
    SharedImageBuffer                   *_sharedImageBuffer;
    QScopedPointer<CaptureThread>       _captureThread;
    QScopedPointer<ProcessingThread>    _processingThread;
    QScopedPointer<PluginLoader>        _pluginLoader;

    int                                 _deviceNumber;
    int                                 _imageBufferSize;
    nooba::VideoState                   _vidState;
    ProcParams                          _params;
    QTimer                              _timer;
    FrameViewer                         _inputWind;
    OutputWind                          *_debugOutWind;
    ParamConfigWind                     *_paramConfigUI;
    StatPanel                           *_statPanel;
    QMap<NoobaPlugin*, QMap<QString, MdiSubWindData*> >  _frameViewerMap;
};

#endif // CAMERAVIEW_H
