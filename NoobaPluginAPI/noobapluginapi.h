#ifndef NOOBAPLUGINAPI_H
#define NOOBAPLUGINAPI_H

#include "NoobaPluginAPI_global.h"
#include "noobapluginbase.h"


#include <QString>
#include <QtPlugin>

#include "property.h"

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

/*
 * Structure to output extra information other than the output image.
 */
struct ProcParams
{
    ProcParams():_ok(true) {}

    int         _frameId;   // unique frameId for the video
    QString     _data;      // can be plain text, xml, csv etc
    bool        _ok;        // frame process status, successful or not
    QString     _err;       // error details
};

struct PluginInfo
{
    PluginInfo(const QString& name, const int majorVersion, const int minorVersion, const QString& description):
        _name(name),
        _description(description),
        _majorVersion(majorVersion),
        _minorVersion(minorVersion) {}

        QString     _name;          // name of the plugin
        QString     _description;   // small explanation of the plugin
        QString     _author;        // creator(s) of the plugin

       /* version numbers of a processing class has a major version and a minor version
        * eg: 1.5 , here 1 is the major version and 5 is the minor version
        */
        int         _majorVersion; // major version of plugin, different from API version
        int         _minorVersion; // minor version of plugin, different from API version
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
    virtual void init() = 0;

    /**
     * @brief release   This member function is called before unloading the plugin. Resources that need to be
     *                  released before unloading need to be called here.
     */
    virtual void release() = 0;
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

protected:

    /* private constructor so that this class could never be instantiated. Only be casted to this
       interface type to get the API version details.
     */
    NoobaPluginAPI(): NoobaPluginAPIBase() {}

private:

    NoobaPluginAPI(const NoobaPluginAPI& rhs);                // private copy constructor
    NoobaPluginAPI& operator=(const NoobaPluginAPI& rhs);     // private assignment operator
};

Q_DECLARE_INTERFACE(NoobaPluginAPI, "NoobaVSS.NoobaPluginAPI/" );
#endif // NOOBAPLUGINAPI_H
