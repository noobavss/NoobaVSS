#ifndef NOOBAPLUGIN_H
#define NOOBAPLUGIN_H

// Qt
#include <QObject>
#include <QMap>
#include <QMetaType>
#include <QStringList>
#include <QRectF>
#include <QPointF>
#include <QImage>

#include "noobapluginapi.h"

// forward declerations
class QPluginLoader;

namespace cv { class Mat; }

struct IntData
{
    /**
     * @brief IntData
     * @param varName
     * @param val
     * @param max
     * @param min
     */
    IntData(const QString& varName, int val, int max, int min):
        _val(val), _max(max), _min(min), _varName(varName) {}

    int             _val;
    int             _max;
    int             _min;
    const QString   _varName;
};

Q_DECLARE_METATYPE(IntData*)     // can be used with QVariant

struct DoubleData
{
    /**
     * @brief DoubleData
     * @param varName
     * @param val
     * @param max
     * @param min
     */
    DoubleData(const QString& varName, double val, double max, double min):
        _val(val), _max(max), _min(min), _varName(varName) {}
    double          _val;
    double          _max;
    double          _min;
    const QString   _varName;
};

Q_DECLARE_METATYPE(DoubleData*)     // can be used with QVariant

struct StringData
{
    /**
     * @brief StringData
     * @param varName
     * @param val
     * @param isFilePath
     */
    StringData(const QString& varName, const QString& val, bool isFilePath):
        _isFilePath(isFilePath), _varName(varName), _val(val) {}

    bool            _isFilePath;
    const QString   _varName;
    QString         _val;
};

Q_DECLARE_METATYPE(StringData*)     // can be used with QVariant

struct StringListData
{
    /**
     * @brief StringListData
     * @param varName
     * @param varList   String list (String list must be non-empty)
     */
    StringListData(const QString& varName, const QStringList& varList ):
        _varName(varName), _varList(varList) { _val = varList.first(); }

    const QString       _varName;
    const QStringList   _varList;
    QString             _val;
};

Q_DECLARE_METATYPE(StringListData*)     // can be used with QVariant

struct PointData {

    PointData(const QString& varName, const QPointF& point):
        _varName(varName), _val(point){}

    const QString       _varName;
    QPointF             _val;
};

Q_DECLARE_METATYPE(PointData*)

struct RectData {

    RectData(const QString& varName, const QRectF& region):
        _varName(varName), _val(region) {}

    const QString       _varName;
    QRectF              _val;
};

Q_DECLARE_METATYPE(RectData*)

struct FrameViewerData {

    FrameViewerData(const QString& title, bool isVisible):
        _title(title), _isVisible(isVisible) {}

    QString _title;
    QImage  _img;
    bool    _isVisible;
};

Q_DECLARE_METATYPE(FrameViewerData*)

struct FilePathData {

    FilePathData(const QString& varName, const QString& path, const QString& filter, nooba::PathType pathType):
        _varName(varName), _val(path), _filter(filter), _pathType(pathType) {}

    QString             _varName;
    QString             _val;
    QString             _filter;
    nooba::PathType     _pathType;
};

Q_DECLARE_METATYPE(FilePathData*)

/**
 * @brief The NoobaPlugin class this is a wrapper class for the Loaded plugins
 */
class NoobaPlugin : public QObject
{
    Q_OBJECT

public:

    explicit NoobaPlugin(const QString& fileName, const QString& alias, NoobaPluginAPI* api, QPluginLoader* loader, QObject *parent = 0);
    ~NoobaPlugin();

    bool init();
    bool release();
    bool procFrame(const cv::Mat& in, cv::Mat& out, ProcParams& params);
    QPluginLoader* getPluginLoader() const { return _pluginLoader; }
    bool isBasePlugin() const {return _isBasePlugin; }
    void setIsBasePlugin(bool isBase) { _isBasePlugin = isBase; }

    /**
     * @brief getFileName local filename of the plugin file is returned
     * @return
     */
    QString fileName() const { return _fileName; }

    /**
     * @brief alias     Name given for the plugin at loading is returned. This is not the plugin name
     *                  but the name given at program runtime. This makes it easy to identify plugins with
     *                  same name loaded twice.
     * @return
     */
    QString alias() const { return _alias; }

    /**
     * @brief getPluginInfo plugin related metadata is returned
     * @return
     */
    PluginInfo getPluginInfo() const;

    const QMap<QString, IntData* >& getIntParamMap() const { return _intMap; }
    const QMap<QString, DoubleData* >& getDoubleParamMap() const { return _doubleMap; }
    const QMap<QString, StringData* >& getStringMap() const { return _stringMap; }
    const QMap<QString, StringListData* >& getStringListMap() const { return _stringListMap; }
    const QMap<QString, FilePathData* >& getFilePathDataMap() const { return _filePathDataMap; }
    
signals:
    
    void debugMsg(const QString& msg);
    void outputData(const PluginPassData& data);
    void outputData(const QStringList& strList, QList<QImage> imageList);
    void onInit(NoobaPlugin* plugin);
    void onAboutToRelease(QString alias);
    void createFrameViewer(const QString& title, bool isVisible);
    void createLineParam(const QString& title, const QString& frameViewerTitle);
    void updateFrameViewer(const QString& pluginAlias, const QString& title, const QImage& frame);

    void intParamUpdate(const QString& varName, int val);
    void doubleParamUpdate(const QString& varName, double val);
    void stringParamUpdate(const QString& varName, const QString& val);
    void filePathParamUpdate(const QString& varName, const QString& path);
    void multiValParamUpdate(const QString& varName, const QString& val);
    void pointParamUpdate(const QString& varName, const QPointF& val);
    void rectParamUpdate(const QString& varName, const QRectF& val);
    void setFrameViewerVisibility(const QString& alias, const QString& title, bool isVisible);

public slots:

    // update the values through this function
    void onIntParamUpdate(const QString& varName, int val);
    void onDoubleParamUpdate(const QString& varName, double val);
    void onStringParamUpdate(const QString& varName, const QString& val);
    void onFilePathParamUpdate(const QString& varName, const QString& path);
    void onMultiValParamUpdate(const QString& varName, const QString& val);
    void onPointParamUpdate(const QString& varName, const QPointF& val);
    void onRectParamUpdate(const QString& varName, const QRectF& val);
    void onLineParamUpdate(const QString& varName, const QString& frameViewerTitle, const QPoint& p1, const QPoint& p2);
    void inputData(const PluginPassData& data);
    void inputData(const QStringList& strList, QList<QImage> imageList);
    void saveConfig(const QString& filename);
    void loadPrevConfig();

private slots:

    // on create request of plugins call this function
    void onCreateFrameViewer(const QString& title, bool isVisible);
    void onCreateIntParam(const QString& varName, int val, int max, int min);
    void onCreateDoubleParam(const QString& varName, double val, double max, double min);
    void onCreateStringParam(const QString& varName, const QString& val, bool isFilePath);
    void onCreateFilePathParam(const QString& varName, QString path, nooba::PathType pathType, const QString& filter);
    void onCreateMultiValParam(const QString& varName, const QStringList& varList);
    void onCreatePointParam(const QString& varName, const QPointF& val);
    void onCreateLineParam(const QString& varName, const QString& frameViewerTitle);
    void onCreateRectParam(const QString& varName, const QRectF& val);
    void onDebugMsg(const QString& msg);
    void onUpdateFrameViewerRequest(const QString& title, const QImage& frame);
    void onSetFrameViewerVisibility(const QString& title, bool isVisible);

private:

    template <typename value> void deleteMapItems(QMap<QString, value>& map);

    void initSignalSlots();
    void releaseSignalSlots();

    bool                            _isBasePlugin;
    NoobaPluginAPI*                 _api;
    QPluginLoader*                  _pluginLoader;
    QString                         _fileName;
    QString                         _alias;
    // internal data maps
    QMap<QString, IntData* >        _intMap;
    QMap<QString, DoubleData* >     _doubleMap;
    QMap<QString, StringData* >     _stringMap;
    QMap<QString, StringListData* > _stringListMap;
    QMap<QString, PointData* >      _pointMap;
    QMap<QString, RectData* >       _rectMap;
    QMap<QString, FrameViewerData*> _frameViewerDataMap;
    QMap<QString, FilePathData* >   _filePathDataMap;

};

Q_DECLARE_METATYPE(NoobaPlugin*)

#endif // NOOBAPLUGIN_H
