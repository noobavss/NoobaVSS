/**
    @file noobapluginsapi.h
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

#ifndef NOOBAPLUGINAPI_H
#define NOOBAPLUGINAPI_H

#include "noobapluginbase.h"
#include "nooba.h"
#include "procparams.h"
#include "pluginpassdata.h"
#include "plugininfo.h"

//Qt
#include <QObject>
#include <QString>
#include <QStringList>
#include <QtPlugin>
#include <QImage>
#include <QColor>

/************************************************************************/
/* IMPORTANT: ANY CHANGE TO THE API CORRESPOND TO A CHANGE IN THE       */
/*            VERSION NUMBER. SO IF A CHANGE IS DONE TO THE API         */
/*            REMEMBER TO CHANGE THE API MAJOR AND MINOR VESION         */
/*            NUMBERS ACCORDINGLY.                                      */
/*                                                                      */
/* ~ Asitha Nanayakkara <daun07@gmail.com> - 2/3/2013                   */
/************************************************************************/
#define API_MAJOR_VERSION   0
#define API_MINOR_VERSION   12
// forward declaration
namespace cv {
    class Mat;
}

struct NoobaPluginAPIPrivate;

class NoobaPluginAPI: public NoobaPluginAPIBase
{
    Q_OBJECT

public:

    virtual ~NoobaPluginAPI(){}

    int APIMajorVersion() { return API_MAJOR_VERSION; }
    int APIMinorVersion() { return API_MINOR_VERSION; }

    /*!
     * \brief   Initialise the resources used by the Plugin
     *
     *  Setting the plugin related resource aquisitions should be done within this function.
     *  Before plugin is used for processing work this function is called.
     *
     *  Resources that need to be refreshed when reloading need to be initialised within this
     *  function and released in release() function. Some plugins may need to load training data
     *  and need not refresh that at a later time. In that case training a model can be done in
     *  the constructor.
     *
     *  This function is a pure virtual function. When creating a plugin this function should be
     *  implemented in the plugin
     *
     * \sa release()
     */
    virtual bool init() = 0;

    /*!
     * \brief releases the resources used by the plugin.
     *
     *  This function is called before unloading the plugin. Resources that need to be
     *  released before unloading need to be done here. This functions is introduced to make
     *  plugins release its resources without distructing the loaded plugins in the application.
     *
     *  When reloading a plugin first release() function is called then init() is called. This is
     *  much efficient than destructing and creating plugins. If some of the state variable of
     *  the plugin needs to be preserved when reloading, that can be done by simply
     *  not releasing those variable data in release() function.
     *
     *  This function is a pure virtual function. When creating a plugin this function should be
     *  implemented in the plugin
     *
     *  \sa init(), NoobaPluginAPI()
     */
    virtual bool release() = 0;

    /*!
     * \brief Base plugins' processing is done by calling this function.
     *
     *  When the plugin is the base plugin this function is called by the Nooba FE application
     *  to process frames.
     *
     *  It is recommended to use a seperate private processing member function within the plugin
     *  to process data. Inside this function calling the private member function is recommended.
     *
     *  This function is a pure virtual function. When creating a plugin this function should be
     *  implemented in the plugin
     *
     * \param in        input image as a opencv mat object
     * \param out       output image
     * \param params    ProcParams struct reference
     * \return boolean true should be return on successful completion of processing a frame, and
     *          false otherwise
     *
     *  \sa inputData(), ProcParams;
     */
    virtual bool procFrame(const cv::Mat& in, cv::Mat& out, ProcParams& params) = 0;

    /*!
     * \brief metadata related to the plugin is taken from the function
     *
     * \return PluginInfo structure is returned.
     */
    virtual PluginInfo getPluginInfo() const = 0;

    // functions to create variables that need to be changed by the user.
    // once created user can see the variables and change them.

    /*!
     * \brief integer parameters can be created using this function
     *
     * This will create a parameter of type integer. This is a int type in C++. This parameter is of
     * nooba::IntParam type.
     * User can change the value of this parameter through the nooba VSS frontend. Changes to this
     * function by the user interface is notified back to the plugin by onIntParamChanged() function.
     * This function should be implemented in the plugin to listen for the changes done to
     * nooba::IntParam type variables.
     *
     * @note This function should be called in init() function. if not this may lead to unexpected behaviour.
     *
     * \param varName variable name of the parameter. this is the variable name refered to in onIntParamChanged()
     * \param val   default value of the integer parameter
     * \param max   maximum possible value for the parameter. If not given 100 is used
     * \param min   minimum possible value for the parameter. If not given 0 is used
     *
     * \sa onIntParamChanged();
     */
    void createIntParam(const QString& varName, int val, int max = 100, int min = 0)
    {   emit createIntParamRequest(varName, val, max, min);}

    /**
     * \brief double parameters can be created using this function
     *
     * This will create a parameter of type double. This is double type in C++. This parameter is of
     * nooba::DoubleParam type.
     * User can change the value of this parameter through the nooba VSS frontend. Changes to this
     * function by the user interface is notified back to the plugin by onDoubleParamChanged() function.
     * This function should be implemented in the plugin to listen for the changes done to
     * nooba::DoubleParam type variables.
     *
     * @note This function should be called in init() function. if not this may lead to unexpected behaviour.
     *
     * \param varName variable name of the parameter. this is the variable name refered to in onDoubleParamChanged()
     * \param val   default value
     * \param max   maximum possible value for the parameter. If not given 100 is used
     * \param min   minimum possible value for the parameter. If not given 0 is used
     *
     * \sa onDoubleParamChanged();
     */
    void createDoubleParam(const QString& varName, double val, double max = 100.0, double min = 0.0)
    {   emit createDoubleParamRequest(varName, val, max, min); }

    /*!
     * \brief String parameters can be created using this function
     *
     * This will create a parameter of type QString. This is QString type in Qt C++. This parameter is of
     * nooba::StringParam type if isFilePath argument is false and of nooba::FilePathParam otherwise
     * If it is a file path it is preferred to use a createFilePathParam() function. This
     * file path functionality is kept purely for compatibility purposes.
     *
     * @note This function should be called in init() function. if not this may lead to unexpected behaviour.
     *
     * User can change the value of this parameter through the nooba VSS frontend. Changes to this
     * function by the user interface is notified back to the plugin by onStringParamChanged() function.
     * This function should be implemented in the plugin to listen for the changes done to
     * nooba::StringParam type variables or nooba::FilePathParam type variables
     *
     * \param varName variable name of the parameter. this is the variable name refered to in onStringParamChanged()
     * \param val   default value
     * \param isFilePath whether to consider the parameter as a filepath or not.
     *
     * \sa onStringParamChanged(), createFilePathParam();
     */
    void createStringParam(const QString& varName, QString val, bool isFilePath = false)
    {   emit createStringParamRequest(varName, val, isFilePath);}

    /*!
     * \brief filePath parameters can be created using this function
     *
     * User can change the value of this parameter through the nooba VSS frontend. Changes to this
     * function by the user interface is notified back to the plugin by onFilePathParamChanged() function.
     * This function should be implemented in the plugin to listen for the changes done to this parameter
     *
     * @note This function should be called in init() function. if not this may lead to unexpected behaviour.
     *
     * \param varName   variable name of the parameter. this is the variable name refered to in onFilePathParamChanged()
     * \param path      default path
     * \param pathType  Type of selection is set according to nooba::PathType
     * \param filter    filter the selectable paths. following example shows including multiple filters
     *                  eg: "Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"
     *
     * \sa onFilePathParamChanged();
     */
    void createFilePathParam(const QString& varName, QString path = QString(), nooba::PathType pathType = nooba::FilePath, const QString& filter = QString() )
    {   emit createFilePathParamRequest(varName, path, pathType, filter); }

    /*!
     * \brief Multi value Parameters can be created using this function.
     *
     * This type of variable can be used to create variable where there is a predefined set of values to
     * choose from. When creating the parameter each value should be represented in QString format
     *
     * @note This function should be called in init() function. if not this may lead to unexpected behaviour.
     *
     * \param varName variable name of the parameter. this is the variable name refered to in onMultiValParamChanged()
     * \param varList list of string containing the options for the user to select from.
     *
     * \sa onMultiValParamChanged();
     */
    void createMultiValParam(const QString& varName, const QStringList& varList)
    {   emit createMultiValParamRequest(varName, varList);   }

    /*!
     * \brief Line parameters can be created using this function
     *
     * @note This function should be called in init() function. If not this may lead to unexpected behaviour.
     *
     * \param varName variable name of the parameter. this is the variable name refered to in onLineParamUpdated()
     * \param frameViewerTitle  title of the frameViewer that this line drawing tool
     *                          should be shown on.
     */
    void createLineParam(const QString& varName, const QString& frameViewerTitle, QColor lineColor)
    {   emit createLineParamRequest(varName, frameViewerTitle, lineColor); }

    /**
     * debug output messages can be sent using this
     */
    /*!
     * \brief debugMsg
     * \param msg
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

    /**
     * @brief generateAlert generate an alert.
     * @param alert
     */
    void generateAlert(const QString& frameViewerTitle, const QString& msg,  nooba::AlertType alert)  // after version 0.11
    {   emit generateAlertRequest(frameViewerTitle, msg, alert); }

signals:

    void createIntParamRequest(const QString& varName, int val, int max = 100, int min = 0);
    void createDoubleParamRequest(const QString& varName, double val, double max = 100.0, double min = 0.0);
    void createStringParamRequest(const QString& varName, QString val, bool isFilePath = false);
    void createMultiValParamRequest(const QString& varName, const QStringList& varList);
    void createLineParamRequest(const QString& varName, const QString& frameViewerTitle, QColor lineColor);   // after version 0.10
    void debugMsgRequest(const QString& msg);
    void outputDataRequest(const PluginPassData& data);
    void outputDataRequest(const QStringList& strList, QList<QImage> imageList);
    void createFrameViewerRequest(const QString& title, bool isVisible = true); // added isVisible argument after ver 0.9
    void updateFrameViewerRequest(const QString& title, const QImage& frame);
    void updateFrameViewerVisibilityRequest(const QString& title, bool isVisible); // after ver 0.9
    void createFilePathParamRequest(const QString& varName, QString path,
                                    nooba::PathType pathType, const QString& filter);   // added in version 0.10
    void generateAlertRequest(const QString& frameViewerTitle, const QString& msg, nooba::AlertType alert);  // after version 0.11

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

    virtual void onLineParamUpdated(const QString& varName, const QString frameViewerTitle, QLine line)
    {   Q_UNUSED(varName) Q_UNUSED(frameViewerTitle) Q_UNUSED(line) }

protected:

   /**
     * private constructor so that this class could never be instantiated. Only be casted to this
     * interface type to get the API version details.
     */
    explicit NoobaPluginAPI();

private:

    NoobaPluginAPI(const NoobaPluginAPI& rhs);                // private copy constructor
    NoobaPluginAPI& operator=(const NoobaPluginAPI& rhs);     // private assignment operator

    NoobaPluginAPIPrivate*      d;                            // d pointer (opaque pointer)
};

Q_DECLARE_INTERFACE(NoobaPluginAPI, "NoobaVSS.NoobaPluginAPI/" )
#endif // NOOBAPLUGINAPI_H
