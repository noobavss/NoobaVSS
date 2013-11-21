/************************************************************************/
/* qt-opencv-multithreaded:                                             */
/* A multithreaded OpenCV application using the Qt framework.           */
/*                                                                      */
/* ProcessingThread.cpp                                                 */
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

#include "ProcessingThread.h"
#include "MatToQImage.h"
#include "PluginLoader.h"
#include "NoobaPlugin.h"

ProcessingThread::ProcessingThread(SharedImageBuffer *sharedImageBuffer, int deviceNumber) :
    QThread(),
    _sharedImageBuffer(sharedImageBuffer),
    _pluginLoader(NULL)
{
    // Save Device Number
    _deviceNumber = deviceNumber;
    // Initialize members
    _doStop=false;
}

void ProcessingThread::run()
{        
    _pluginLoader = new PluginLoader();
    FrameProcessor frameProc(_sharedImageBuffer, _pluginLoader, _deviceNumber);

    qRegisterMetaType< struct ThreadStatisticsData>("ThreadStatisticsData");
    connect(&frameProc, SIGNAL(updateStatisticsInGUI(ThreadStatisticsData)), this, SIGNAL(updateStatisticsInGUI(ThreadStatisticsData)));

    qRegisterMetaType< NoobaPlugin* >("NoobaPlugin");
    connect(_pluginLoader, SIGNAL(pluginLoaded(NoobaPlugin*)), this, SIGNAL(pluginLoaded(NoobaPlugin*)));
    connect(_pluginLoader, SIGNAL(pluginInitialised(NoobaPlugin*)), this, SIGNAL(pluginInitialised(NoobaPlugin*)));
    connect(_pluginLoader, SIGNAL(pluginAboutToUnloaded(QString)), this, SIGNAL(pluginAboutToUnload(QString)));
    connect(_pluginLoader, SIGNAL(pluginAboutToRelease(QString)), this, SIGNAL(pluginAboutToRelease(QString)));
    connect(_pluginLoader, SIGNAL(basePluginChanged(NoobaPlugin*)), this, SIGNAL(basePluginChanged(NoobaPlugin*)));
    connect(_pluginLoader, SIGNAL(pluginLoaded(NoobaPlugin*)), this, SLOT(onPluginLoaded(NoobaPlugin*)), Qt::DirectConnection);

    qRegisterMetaType< struct PluginConnData* >("PluginConnData");
    connect(_pluginLoader, SIGNAL(pluginsConnected(PluginConnData*)), this, SIGNAL(pluginsConnected(PluginConnData*)));
    connect(_pluginLoader, SIGNAL(pluginsDisconnected(PluginConnData*)), this, SIGNAL(pluginsDisconnected(PluginConnData*)));
    connect(this, SIGNAL(FrameAddedToImageBuffer()), &frameProc, SLOT(processFrame()));
    connect(&frameProc, SIGNAL(inputFrame(QImage)), this, SIGNAL(inputFrame(QImage)));
    // after connecting plugin loader signals
    _pluginLoader->loadPluginInfo();
    _pluginLoader->loadPrevConfig();

    qDebug() << tr("[ %1 ] Starting processing thread").arg(_deviceNumber);
    exec();

    /**
      Plugin loader deleted
      */
    pluginLoaderMutex.lock();
    _pluginLoader->deleteLater();
    _pluginLoader = NULL;
    pluginLoaderMutex.unlock();
    /**
      unloc mutex
      */
    qDebug() << tr("[ %1 ] Stopping processing thread").arg(_deviceNumber);
}

PluginLoader *ProcessingThread::getPluginLoader()
{
    // TODO Refine this to not block the whole access but
    // to make sure vital operations the synchronised
    QMutexLocker locker(&pluginLoaderMutex);
    qDebug() << currentThreadId() << Q_FUNC_INFO;
    return _pluginLoader;
}

void ProcessingThread::onPluginLoaded(NoobaPlugin *plugin)
{
    // NOTE: WHY THIS SLOT?
    // Plugin loading and initialisation done in Processing thread. GUI thread waiting for this signal to
    // connect signals emited at plugin initialisation will miss the signals since signals emitted on
    // initialisation will be emitted before the connection is made in the GUI thread.
    // To overcome this issue the signal passing is transfered to the processing thread itself using this
    // directly connected slot.
    connect(plugin, SIGNAL(createFrameViewer(QString, bool)), this, SLOT(onCreateFrameRequest(QString, bool)));
    connect(plugin, SIGNAL(debugMsg(QString)), this, SIGNAL(debugMsg(QString)));
}

void ProcessingThread::onCreateFrameRequest(const QString &title, bool isVisible)
{
    NoobaPlugin* p = qobject_cast<NoobaPlugin* >(sender());
    if(!p)
        return;

    emit createFrameViewer(title, isVisible, p);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//
// Class FrameProcessor
//
//////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief ProcessInput::ProcessInput
 * @param sharedImageBuffer
 * @param pluginLoader
 * @param deviceNumber
 * @param parent
 */
FrameProcessor::FrameProcessor(SharedImageBuffer *sharedImageBuffer, PluginLoader *pluginLoader, int deviceNumber, QObject *parent)
    : QObject(parent),
      _deviceNumber(deviceNumber),
      _sharedImageBuffer(sharedImageBuffer),
      _pluginLoader(pluginLoader)
{
    fpsSum = 0;
    fps.clear();
    _sampleNumber = 0;
    statsData.averageFPS = 0;
    statsData.nFramesProcessed = 0;
}

FrameProcessor::~FrameProcessor()
{
}

void FrameProcessor::processFrame()
{
    QMutexLocker locker(&processingMutex);

    int processingTime = t.elapsed();
    // Start timer (used to calculate processing rate)
    t.start();

    Buffer<Mat> *buffer = _sharedImageBuffer->getByDeviceNumber(_deviceNumber);
    if(!buffer)
    {
        qDebug() << tr("Buffer is null.") << Q_FUNC_INFO;
        return;
    }

    if( buffer->isEmpty())
    {
        return;
    }
    // Get frame from queue, store in currentFrame
    currentFrame = buffer->get().clone();
    emit inputFrame(MatToQImage(currentFrame));
    // Example of how to grab a frame from another stream (where Device Number=1)
    // Note: This requires stream synchronization to be ENABLED (in the Options menu of MainWindow) and frame processing for the stream you are grabbing FROM to be DISABLED.
    /*
    if(sharedImageBuffer->containsImageBufferForDeviceNumber(1))
    {
        // Grab frame from another stream (connected to camera with Device Number=1)
        Mat frameFromAnotherStream = Mat(sharedImageBuffer->getByDeviceNumber(1)->getFrame(), currentROI);
        // Linear blend images together using OpenCV and save the result to currentFrame. Note: beta=1-alpha
        addWeighted(frameFromAnotherStream, 0.5, currentFrame, 0.5, 0.0, currentFrame);
    }
    */
    NoobaPlugin* p = _pluginLoader->getBasePlugin();
    if(!p)  // wait till the base blugin loaded
        return;
    cv::Mat out;
    ProcParams params;
    p->procFrame(currentFrame, out, params);

     // Update statistics
    updateFPS(processingTime);
    statsData.nFramesProcessed++;
    // Inform GUI of updated statistics
    emit updateStatisticsInGUI(statsData);
    return;
}

void FrameProcessor::updateFPS(int timeElapsed)
{
    // Add instantaneous FPS value to queue
    if(timeElapsed>0)
    {
        fps.enqueue((int)1000/timeElapsed);
        // Increment sample number
        _sampleNumber++;
    }

    // Maximum size of queue is DEFAULT_PROCESSING_FPS_STAT_QUEUE_LENGTH
    if(fps.size() > PROCESSING_FPS_STAT_QUEUE_LENGTH)
        fps.dequeue();

    // Update FPS value every DEFAULT_PROCESSING_FPS_STAT_QUEUE_LENGTH samples
    if((fps.size() == PROCESSING_FPS_STAT_QUEUE_LENGTH)&&(_sampleNumber==PROCESSING_FPS_STAT_QUEUE_LENGTH))
    {
        // Empty queue and store sum
        while(!fps.empty())
            fpsSum += fps.dequeue();
        // Calculate average FPS
        statsData.averageFPS=fpsSum/PROCESSING_FPS_STAT_QUEUE_LENGTH;
        // Reset sum
        fpsSum = 0;
        // Reset sample number
        _sampleNumber = 0;
    }
}
