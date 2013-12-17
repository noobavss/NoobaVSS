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
#ifndef PLUGININFO_H
#define PLUGININFO_H

#include <QString>

struct PluginInfoPrivate;

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

#endif // PLUGININFO_H
