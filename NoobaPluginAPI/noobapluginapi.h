#ifndef NOOBAPLUGINAPI_H
#define NOOBAPLUGINAPI_H

//#include "NoobaPluginAPI_global.h"
#include "noobapluginbase.h"

//Qt
#include <QObject>
#include <QString>
#include <QStringList>
#include <QtPlugin>
#include <QMetaType>
#include <QImage>

/************************************************************************/
/* IMPORTANT: ANY CHANGE TO THE API CORRESPOND TO A CHANGE IN THE       */
/*            VERSION NUMBER. SO IF A CHANGE IS DONE TO THE API         */
/*            REMEMBER TO CHANGE THE API MAJOR AND MINOR VESION         */
/*            NUMBERS ACCORDINGLY.                                      */
/*                                                                      */
/* ~ Asitha Nanayakkara <daun07@gmail.com> - 2/3/2013                   */
/************************************************************************/

// forward declaration
namespace cv {
    class Mat;
}

namespace nooba {

    enum ParamType {
        IntParam        = 0,
        DoubleParam     = 1,
        StringParam     = 2,
        RangeParam      = 3,
        RegionParam     = 4,
        MultiValueParam = 5,
        FilePathParam   = 6
    };

    /* Enum to define the current state of the video
     */
    enum VideoState {
        PlayingState    = 0,
        PausedState     = 1,
        StoppedState    = 2
    };

    enum NoobaAlert {
        RedAlert    = 0,
        AmberAlert  = 1
    };

    enum PathType {
        FilePath    = 0,
        DirPath     = 1
    };
}

Q_DECLARE_METATYPE(nooba::ParamType)   // now ParamTypes can be used with QVariants

struct ProcParamsPrivate;
struct PluginInfoPrivate;
struct NoobaPluginAPIPrivate;
struct PluginPassDataPrivate;

/*
 * Structure to output extra information other than the output image.
 */
class ProcParams
{
public:

    ProcParams();
    ProcParams(const ProcParams& rhs);                // copy constructor
    ProcParams& operator=(const ProcParams& rhs);     // assignment operator

    ~ProcParams();

    void setFrameId(int id);
    int frameId() const;

    void setFrameRate(double rate);
    double frameRate() const;

    void setErrorState(bool isError);
    bool isError() const;

    void setErrorMsg(const QString& errMsg);
    QString errMsg() const;

    /**
     * @brief currentVideoState gives the current video state
     * @return
     */
    nooba::VideoState currentVideoState() const;

    /**
     * @brief setVideoState Currently Nothing happens by calling this. Only changes the internal variable.
     *                      No effect on the actual state.
     * @param state
     * @return
     */
    bool setVidState(nooba::VideoState state);

private:

    ProcParamsPrivate* createPrivateStruct(const ProcParams& rhs);
    ProcParamsPrivate*   d;  // d pointer

};

Q_DECLARE_METATYPE(ProcParams*)

class PluginPassData
{
public:

    explicit PluginPassData();
    ~PluginPassData();

    PluginPassData(const PluginPassData& rhs);                // copy constructor
    PluginPassData& operator=(const PluginPassData& rhs);     // assignment operator
    QStringList strList() const;
    void setStrList(const QStringList& list);
    void appendStrList(const QString& str);
    void setImage(const QImage& image);
    QImage& getImage() const;

private:

    PluginPassDataPrivate* createPrivateStruct(const PluginPassData& rhs);
    PluginPassDataPrivate*  d; // d pointer
};

Q_DECLARE_METATYPE(PluginPassData*)
Q_DECLARE_METATYPE(PluginPassData)

/**
 * @brief Plugin information are stored in this structure
 */
class PluginInfo
{
public:

    PluginInfo(const QString& name, const int majorVersion, const int minorVersion,
               const QString& description, const QString& author);
    PluginInfo(const PluginInfo& rhs);                // copy constructor
    PluginInfo& operator=(const PluginInfo& rhs);     // assignment operator

    ~PluginInfo();

    QString name() const;
    QString description() const;
    QString author() const;

    int majorVersion() const;
    int minorVersion() const;

private:

    PluginInfoPrivate *createPrivateStruct(const PluginInfo& rhs);
    PluginInfoPrivate*  d;

};

class NoobaPluginAPI: public NoobaPluginAPIBase
{
    Q_OBJECT

public:

    virtual ~NoobaPluginAPI(){}

    /**
     * @brief   Initialise the plugins properties. Setting the plugin related properties should be done
     *          within this function.
     *          use setProperty(const Property& property)
     *
     */
    virtual bool init() = 0;

    /**
     * @brief release   This member function is called before unloading the plugin. Resources that need to be
     *                  released before unloading need to be called here.
     */
    virtual bool release() = 0;
    /**
     * @brief Each Plugin has to implement this function. Each frame is processed using this function.
     * @param in        input image as a opencv mat object
     * @param out       output image
     * @param params    ProcParams struct reference
     * @return
     */
    virtual bool procFrame(const cv::Mat& in, cv::Mat& out, ProcParams& params) = 0;

    /**
     * @brief
     * @return the plugin information
     */
    virtual PluginInfo getPluginInfo() const = 0;

    // functions to create variables that need to be changed by the user.
    // once created user can see the variables and change them.

    /**
     * @brief createIntParam integer parameter created
     * @param varName
     * @param val
     * @param max
     * @param min
     */
    void createIntParam(const QString& varName, int val, int max = 100, int min = 0)
    {   emit createIntParamRequest(varName, val, max, min);}

    /**
     * @brief createDoubleParam     double parameter created
     * @param varName
     * @param val
     * @param max
     * @param min
     */
    void createDoubleParam(const QString& varName, double val, double max = 100.0, double min = 0.0)
    {   emit createDoubleParamRequest(varName, val, max, min); }

    /**
     * @brief createStringParam     parameter that can take string variables is created.
     * File paths can also be taken using this TODO: File path functionality is yet to be implemented
     * @param varName
     * @param val
     * @param isFilePath
     */
    void createStringParam(const QString& varName, QString val, bool isFilePath = false)
    {   emit createStringParamRequest(varName, val, isFilePath);}

//

    /**
     * @brief createFilePathParam
     * @param varName
     * @param path      default path
     * @param pathType  Type of selection
     * @param filter    filter the selectable paths. following example shows including multiple filters
     *                  eg: "Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"
     */
    void createFilePathParam(const QString& varName, QString path = QString(), nooba::PathType pathType = nooba::FilePath, const QString& filter = QString() )
    {   emit createFilePathParamRequest(varName, path, pathType, filter); }

    /**
     * @brief createMultiValParam Parameter that can be used to create a defined set of strings.
     *  User will be able to select one from this strings.
     * @param varName
     * @param varList
     */
    void createMultiValParam(const QString& varName, const QStringList& varList)
    {   emit createMultiValParamRequest(varName, varList);   }

    void createPointParam(const QString& varName, const QPointF& val)
    {   emit createPointParamRequest(varName, val); }

    void createRectParam(const QString& varName, const QRectF& val)
    {   emit createRectParam(varName, val); }

    /**
     * debug output messages can be sent using this
     */
    void debugMsg(const QString& msg)
    {   emit debugMsgRequest(msg); }

    /**
     * @brief outputData data to be shared with other plugins
     * @param data
     */
    void outputData(const PluginPassData& data)
    {   emit outputDataRequest(data); }

    void outputData(const QStringList& strList, QList<QImage> imageList)
    {   emit outputDataRequest(strList, imageList); }

    /**
     * @brief createOutputWind creates MDI SubWindow to show the frame outputs
     * @param title title of the subWindow
     */
    void createFrameViewer(const QString& title, bool isVisible = true)
    {   emit createFrameViewerRequest(title, isVisible); }

    /**
     * @brief updateFrameViewer
     * @param title
     * @param frame QImage
     */
    void updateFrameViewer(const QString& title, const QImage& frame)
    {   emit updateFrameViewerRequest(title, frame); }

    /**
     * @brief setFrameViewerVisibility
     * @param isVisible seting this to true will make the viewer visible and vice versa
     */
    void setFrameViewerVisibility(const QString& title, bool isVisible)
    {   emit updateFrameViewerVisibilityRequest(title, isVisible); }

signals:

    void createIntParamRequest(const QString& varName, int val, int max = 100, int min = 0);
    void createDoubleParamRequest(const QString& varName, double val, double max = 100.0, double min = 0.0);
    void createStringParamRequest(const QString& varName, QString val, bool isFilePath = false);
    void createMultiValParamRequest(const QString& varName, const QStringList& varList);
    void createPointParamRequest(const QString& varName, const QPointF& val);
    void createRectParamRequest(const QString& varName, const QRectF& val);
    void debugMsgRequest(const QString& msg);
    void outputDataRequest(const PluginPassData& data);
    void outputDataRequest(const QStringList& strList, QList<QImage> imageList);
    void createFrameViewerRequest(const QString& title, bool isVisible = true); // added isVisible argument after ver 0.9
    void updateFrameViewerRequest(const QString& title, const QImage& frame);
    void updateFrameViewerVisibilityRequest(const QString& title, bool isVisible); // after ver 0.9
    void createFilePathParamRequest(const QString& varName, QString path,
                                    nooba::PathType pathType, const QString& filter);   // added in version 0.10

public slots:

    /**
     * This functions will be called when the parameters are changed by the
     * user.
     */
    virtual void onIntParamChanged(const QString& varName, int val) {
        Q_UNUSED(varName) Q_UNUSED(val)
    }

    virtual void onDoubleParamChanged(const QString& varName, double val) {
        Q_UNUSED(varName) Q_UNUSED(val)
    }

    virtual void onStringParamChanged(const QString& varName, const QString& val){
        Q_UNUSED(varName) Q_UNUSED(val)
    }

    virtual void onMultiValParamChanged(const QString& varName, const QString& val){
        Q_UNUSED(varName) Q_UNUSED(val)
    }

    virtual void onPointParamChanged(const QString& varName, const QPointF& val){
        Q_UNUSED(varName) Q_UNUSED(val)
    }

    virtual void onRectParamChanged(const QString& varName, const QRectF& val){
        Q_UNUSED(varName) Q_UNUSED(val)
    }

    virtual void onFilePathParamChanged(const QString& varName, const QString& path) {
        Q_UNUSED(varName) Q_UNUSED(path)
    }

    /**
     * @brief inputData data received from other plugin thru thid function. This is used to interconnect plugins.
     *                  Another plugins outputData(PluginPassData* data) can be connected to this slot.
     * @param data
     */
    virtual void inputData(const PluginPassData& data){
        Q_UNUSED(data)
    }

    virtual void inputData(const QStringList& strList, QList<QImage> imageList){
        Q_UNUSED(strList) Q_UNUSED(imageList)
    }


protected:

   /**
     * private constructor so that this class could never be instantiated. Only be casted to this
     * interface type to get the API version details.
     */
    explicit NoobaPluginAPI();

private:

    NoobaPluginAPI(const NoobaPluginAPI& rhs);                // private copy constructor
    NoobaPluginAPI& operator=(const NoobaPluginAPI& rhs);     // private assignment operator

    NoobaPluginAPIPrivate*      d;                            // d pointer
};

Q_DECLARE_INTERFACE(NoobaPluginAPI, "NoobaVSS.NoobaPluginAPI/" )
#endif // NOOBAPLUGINAPI_H
