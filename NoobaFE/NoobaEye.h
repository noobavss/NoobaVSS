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

#ifndef NOOBAEYE_H
#define NOOBAEYE_H

#include "noobapluginapi.h"

// Qt includes
#include <QMetaType>
#if QT_VERSION >= 0x050000
    #include <qnamespace.h>
#else
    #include <Qt/qnamespace.h>
#endif

namespace nooba
{
    const QString ProgramName   = "Nooba_VSS";
    const QString Organisation  = "Nooba";
    const int  MajorVersion     = 0;
    const int MinorVersion      = 8;

    // QSettings related consts
    const QString Settings_PluginConfig_block   = "Plugin_info";
    const QString Settings_BasePlugin_Filename  = "BasePlugin_filename";
    const QString Settings_ActivePluginList     = "Active_plugin_list";
    const QString Settings_Connection_data      = "Connected_plugin_filenames";
    const QString Settings_InPlug_Alias         = "In";
    const QString Settings_OutPlug_Alias        = "Out";
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
