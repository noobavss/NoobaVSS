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

#include "plugininfo.h"

//////////////////////////////////////////////////////////////
// PluginInfo class and its private data structure

struct PluginInfoPrivate
{
    PluginInfoPrivate(const QString& name, const int majorVersion, const int minorVersion,
                      const QString& description, const QString& author):
        _name(name),
        _description(description),
        _author(author),
        _majorVersion(majorVersion),
        _minorVersion(minorVersion) {}

    // variables
    // ---------------------------------------------------------------
    // NOTE: UPDATE CreatePrivateStruct(...) function on addition of
    //       New variables. Copy and assignment constructors use it.
    // ---------------------------------------------------------------

    QString     _name;          // name of the plugin
    QString     _description;   // small explanation of the plugin
    QString     _author;        // creator(s) of the plugin

   /* version numbers of a processing class has a major version and a minor version
    * eg: 1.5 , here 1 is the major version and 5 is the minor version
    */
    int         _majorVersion; // major version of plugin, different from API version
    int         _minorVersion; // minor version of plugin, different from API version
};

PluginInfo::PluginInfo(const QString &name, const int majorVersion,
                       const int minorVersion, const QString &description, const QString &author)
    : d(new PluginInfoPrivate(name,majorVersion, minorVersion, description, author))
{
}

PluginInfo::PluginInfo(const PluginInfo &rhs)
{
    d = createPrivateStruct(rhs);
}

PluginInfo &PluginInfo::operator =(const PluginInfo &rhs)
{
    if(this == &rhs)
        return *this;

    d = createPrivateStruct(rhs);

    return *this;
}

PluginInfo::~PluginInfo()
{
    delete d;
}

QString PluginInfo::name() const
{
    return d->_name;
}

QString PluginInfo::description() const
{
    return d->_description;
}

QString PluginInfo::author() const
{
    return d->_author;
}

int PluginInfo::majorVersion() const
{
    return d->_majorVersion;
}

int PluginInfo::minorVersion() const
{
    return d->_minorVersion;
}

PluginInfoPrivate* PluginInfo::createPrivateStruct(const PluginInfo &rhs)
{
    return new PluginInfoPrivate( rhs.name(), rhs.majorVersion(), rhs.minorVersion(),
                rhs.description(), rhs.author());
}
