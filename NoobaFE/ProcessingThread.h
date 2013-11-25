/************************************************************************/
/* qt-opencv-multithreaded:                                             */
/* A multithreaded OpenCV application using the Qt framework.           */
/*                                                                      */
/* ProcessingThead.h                                                    */
/*                                                                      */
/* Nick D'Ademo <nickdademo@gmail.com>                                  */
/* Modified to fit NoobaVSS by                                          */
/*      D.A.U.Nanayakkara <daun07@gmail.com>                            */
/*                                                                      */
/* Copyright (c) 2012-2013 Nick D'Ademo                                 */
/*                                                                      */
/* Permission is hereby granted, free of charge, to any person          */
/* obtaining a copy of this software and associated documentation       */
/* files (the "Software"), to deal in the Software without restriction, */
/* including without limitation the rights to use, copy, modify, merge, */
/* publish, distribute, sublicense, and/or sell copies of the Software, */
/* and to permit persons to whom the Software is furnished to do so,    */
/* subject to the following conditions:                                 */
/*                                                                      */
/* The above copyright notice and this permission notice shall be       */
/* included in all copies or substantial portions of the Software.      */
/*                                                                      */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF   */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS  */
/* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   */
/* ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN    */
/* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     */
/* SOFTWARE.                                                            */
/*                                                                      */
/************************************************************************/

#ifndef PROCESSINGTHREAD_H
#define PROCESSINGTHREAD_H

// Qt
#include <QtCore/QThread>
#include <QtCore/QTime>
#include <QtCore/QQueue>
#include <QImage>

// OpenCV
#include <opencv/cv.h>
#include <opencv/highgui.h>
// Local
#include "Structures.h"
#include "Config.h"
#include "Buffer.h"
#include "SharedImageBuffer.h"

using namespace cv;

struct PluginConnData;
class PluginLoader;
class NoobaPlugin;
class QColor;

class FrameProcessor: public QObject
{
    Q_OBJECT

public:
    FrameProcessor(SharedImageBuffer *_sharedImageBuffer, PluginLoader* pluginLoader, int deviceNumber, QObject* parent = 0);
    ~FrameProcessor();

public slots:
    void processFrame();

signals:
    void updateStatisticsInGUI(struct ThreadStatisticsData);
    void inputFrame(const QImage& inputFrame);

private:
    void updateFPS(int timeElapsed);

    int                             fpsSum;
    int                             _deviceNumber;
    int                             _sampleNumber;
    SharedImageBuffer               *_sharedImageBuffer;
    PluginLoader                    *_pluginLoader;
    Mat                             currentFrame;
    Mat                             currentFrameGrayscale;
    Rect                            currentROI;
    QImage                          frame;
    QTime                           t;
    QQueue<int>                     fps;
    QMutex                          processingMutex;
    struct ThreadStatisticsData     statsData;

};

class ProcessingThread : public QThread
{
    Q_OBJECT

    public:
        ProcessingThread(SharedImageBuffer *_sharedImageBuffer, int _deviceNumber);
        QRect getCurrentROI();
        PluginLoader* getPluginLoader();

private:

        void setROI();
        void resetROI();

        SharedImageBuffer               *_sharedImageBuffer;
        PluginLoader                    *_pluginLoader;
        QMutex                          doStopMutex;
        QMutex                          pluginLoaderMutex;
        Size                            frameSize;
        Point                           framePoint;
        struct ImageProcessingFlags     imgProcFlags;
        struct ImageProcessingSettings  imgProcSettings;

        volatile bool                   _doStop;
        int                             _deviceNumber;
        bool                            enableFrameProcessing;

private slots:

        void onPluginLoaded(NoobaPlugin* plugin);

protected:
        void run();

signals:
        void newFrame(const QImage &frame);
        void updateStatisticsInGUI(struct ThreadStatisticsData);
        void inputFrame(const QImage& inputFrame);
        void createFrameViewer(const QString& title, bool isVisible, NoobaPlugin* plugin);
        void createLineParamRequest(const QString& title, const QString& frameViewerTitle, const QColor& lineColor, NoobaPlugin* plugin);
        void debugMsg(const QString& msg);
        void pluginLoaded(NoobaPlugin* plugin);
        void pluginInitialised(NoobaPlugin* plugin);
        void pluginAboutToRelease(QString alias);
        void pluginAboutToUnload(QString alias);
        void basePluginChanged(NoobaPlugin* newBasePlugin);
        void pluginsDisconnected(PluginConnData* pcd);
        void pluginsConnected(PluginConnData* pcd);
        void FrameAddedToImageBuffer();
};

#endif // PROCESSINGTHREAD_H
