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

#ifndef PLUGINPASSDATA_H
#define PLUGINPASSDATA_H

//Qt
#include <QMetaType>
#include <QImage>
#include <QStringList>

struct PluginPassDataPrivate;

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

#endif // PLUGINPASSDATA_H
