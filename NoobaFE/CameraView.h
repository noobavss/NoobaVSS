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
#include <QMutex>

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

public slots:

    bool connectToCamera(int deviceNumber);
    bool connectToVideoFileStream();
    void on_controlButton_clicked();
    void configurePlugins();

private slots:

    void closeEvent(QCloseEvent *event);
    void onInputFrameUpdate(const QImage &in);
    void onFileStreamEOF();

    void onPluginLoad(NoobaPlugin *plugin);
    void onPluginAboutToUnload(QString alias);
    void onPluginInitialised(NoobaPlugin* plugin);
    void onPluginAboutToRelease(QString alias);
    void onCreateFrameViewerRequest(const QString& title, bool isVisible, NoobaPlugin *plugin);
    void onFrameViewerUpdate(const QString& pluginAlias, const QString& title, const QImage& frame);
    void onFrameSetVisibiliy(const QString& alias, const QString &title, bool isVisible);
    void updateCaptureThreadStats(struct ThreadStatisticsData statData);
    void updateProcessingThreadStats(struct ThreadStatisticsData statData);
    void onCreateLineParamRequest(const QString& varName, const QString& frameViewerTitle, QColor lineColor, NoobaPlugin* plugin);
    void onGenerateAlert(const QString& frameViewerTitle, const QString& msg, nooba::AlertType type, NoobaPlugin* plugin);
    void resizeEvent(QResizeEvent *event);
private:

    struct MdiSubWindData
    {
        MdiSubWindData(const QString& pluginAlias, QMdiSubWindow* subWind, FrameViewer* frameViewer)
            :_pluginAlias(pluginAlias), _mdiSubWind(subWind), _frameViewer(frameViewer) {}

        QString         _pluginAlias;
        QMdiSubWindow*  _mdiSubWind;
        FrameViewer*    _frameViewer;
    };

    /*
     *	Updates the current video state
     *  \param state
     */
    void setVideoState(nooba::VideoState state);
    void stopCaptureThread();
    void stopProcessingThread();
    void connectThreadSignalSlots();
    inline void setupSharedBufferForNewDevice();
    QMdiSubWindow *addMDISubWindow(FrameViewer* frameViewer, bool isVisible);

    bool                                _isWebCam;
    Ui::CameraView                      *ui;
    SharedImageBuffer                   *_sharedImageBuffer;
    QScopedPointer<CaptureThread>       _captureThread;
    QScopedPointer<ProcessingThread>    _processingThread;

    int                                 _deviceNumber;
    int                                 _imageBufferSize;
    nooba::VideoState                   _vidState;
    ProcParams                          _params;
    QTimer                              _timer;
    QMutex                              _pluginUpdateMutex;
    FrameViewer                         _inputWind;
    OutputWind                          *_debugOutWind;
    ParamConfigWind                     *_paramConfigUI;
    StatPanel                           *_statPanel;
    QMap<QString, QMap<QString, MdiSubWindData*> >  _frameViewerMap;
};

#endif // CAMERAVIEW_H
