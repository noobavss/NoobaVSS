#ifndef NOOBAPLUGIN_H
#define NOOBAPLUGIN_H

#include <QObject>
#include <QMap>
#include <QMetaType>

// forward declerations
class NoobaPluginAPI;
class ProcParams;
namespace cv { class Mat; }

struct IntData
{
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
    StringData(const QString& varName, const QString& val, bool isFilePath):
        _isFilePath(isFilePath), _varName(varName), _val(val) {}

    bool            _isFilePath;
    const QString   _varName;
    QString         _val;
};

Q_DECLARE_METATYPE(StringData*)     // can be used with QVariant

class NoobaPlugin : public QObject
{
    Q_OBJECT

public:


    explicit NoobaPlugin(NoobaPluginAPI* api, QObject *parent = 0);
    ~NoobaPlugin();

    bool init();
    bool release();
    bool procFrame(const cv::Mat& in, cv::Mat& out, ProcParams& params);

    const QMap<QString, IntData* >& getIntParamMap() const { return _intMap; }
    const QMap<QString, DoubleData* >& getDoubleParamMap() const { return _doubleMap; }
    const QMap<QString, StringData* >& getStringMap() const { return _stringMap; }
    
signals:
    
    void debugMsg(const QString& msg);

public slots:

    void onIntParamUpdate(const QString& varName, int val);
    void onDoubleParamUpdate(const QString& varName, double val);
    void onStringParamUpdate(const QString& varName, const QString& val);

private slots:

    void onCreateIntParam(const QString& varName, int val, int max, int min);
    void onCreateDoubleParam(const QString& varName, double val, double max, double min);
    void onCreateStringParam(const QString& varName, const QString& val, bool isFilePath);

    void onDebugMsg(const QString& msg);

private:

    template <typename Map> void deleteMapItems(Map map);

    void initSignalSlots();
    void releaseSignalSlots();

    NoobaPluginAPI*              _api;
    QMap<QString, IntData* >     _intMap;
    QMap<QString, DoubleData* >  _doubleMap;
    QMap<QString, StringData* >  _stringMap;
};

#endif // NOOBAPLUGIN_H
