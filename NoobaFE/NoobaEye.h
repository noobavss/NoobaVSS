#ifndef NOOBAEYE_H
#define NOOBAEYE_H

#include "noobapluginapi.h"

// Qt includes
#include <QMetaType>
#include <Qt/qnamespace.h>

namespace nooba
{
    const QString ProgramName   = "Nooba_VSS";
    const QString Organisation  = "Nooba";
    const int  MajorVersion     = 0;
    const int MinorVersion      = 2;


    // QSettings related consts
    const QString Settings_PluginInfo_block = "Plugin_info";
    const QString Settings_ActivePlugin_FileName = "ap_filename";

    // plugin information. Used by pluginLoader
    struct PluginData
    {
        // constructor
        PluginData(const QString& fileName, const PluginInfo& pluginInfo, int apiMajor, int apiMinor):
            _apiMajorVersion(apiMajor),
            _apiMinorVersion(apiMinor),
            _fileName(fileName),
            _pluginInfo(pluginInfo) {}

        // data members
        int             _apiMajorVersion;
        int             _apiMinorVersion;
        QString         _fileName;   // name of the dynamic link library file
        PluginInfo      _pluginInfo;
    };

    enum DataRoles         // user roles for data models
    {
        ParamTypeRole   = Qt::UserRole + 1,
        ParamDataRole   = Qt::UserRole + 2
    };
}

/*  Register the nooba structures as Qt Meta types so the structures can be used under QVariants.
    NOTE: See Qt Documentation for QMetaTypes for details. 
 */
Q_DECLARE_METATYPE(nooba::PluginData*)
Q_DECLARE_METATYPE(ProcParams)
//Q_DECLARE_METATYPE(nooba::PluginInfo)

#endif
