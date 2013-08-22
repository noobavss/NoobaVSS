#ifndef NOOBAPLUGINAPI_H
#define NOOBAPLUGINAPI_H

#include "NoobaPluginAPI_global.h"
#include "noobapluginbase.h"
#include "property.h"

#include <QString>
#include <QtPlugin>

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

struct ProcParamsPrivate;
struct PluginInfoPrivate;
struct NoobaPluginAPIPrivate;

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

private:

    ProcParamsPrivate* createPrivateStruct(const ProcParams& rhs);
    ProcParamsPrivate*   d;  // d pointer

};

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

class NOOBAPLUGINAPISHARED_EXPORT NoobaPluginAPI: public NoobaPluginAPIBase
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

    /**
     * @brief errMsg On failure of the plugin this error message should be used to state the error
     * @return QString error details
     */
    QString errMsg() const;

protected:

    /* private constructor so that this class could never be instantiated. Only be casted to this
       interface type to get the API version details.
     */
    NoobaPluginAPI();

    /**
     * @brief setErrMsg on plugin error set the error message
     * @param errMsg details of the error
     */
    void setErrMsg(const QString& errMsg);

private:

    NoobaPluginAPI(const NoobaPluginAPI& rhs);                // private copy constructor
    NoobaPluginAPI& operator=(const NoobaPluginAPI& rhs);     // private assignment operator

    NoobaPluginAPIPrivate*      d;                            // d pointer
};

Q_DECLARE_INTERFACE(NoobaPluginAPI, "NoobaVSS.NoobaPluginAPI/" )
#endif // NOOBAPLUGINAPI_H
