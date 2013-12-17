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

#include "pluginpassdata.h"

///////////////////////////////////////////////////
/// \brief The PluginPassDataPrivate struct
///
struct PluginPassDataPrivate
{
    QStringList     _strList;
    QImage          _img;
};

//////////////////////////////////////////////////
/// \brief PluginPassData::PluginPassData
///
PluginPassData::PluginPassData()
    :d(new PluginPassDataPrivate)
{
}

PluginPassData::~PluginPassData()
{
    delete d;
}

PluginPassData::PluginPassData(const PluginPassData &rhs)
{
    d = createPrivateStruct(rhs);
}

PluginPassData &PluginPassData::operator =(const PluginPassData &rhs)
{
    if(this == &rhs)
        return *this;

    d = createPrivateStruct(rhs);
    return *this;
}

QStringList PluginPassData::strList() const
{
    return d->_strList;
}

void PluginPassData::setStrList(const QStringList &list)
{
    d->_strList = list;
}

void PluginPassData::appendStrList(const QString &str)
{
    d->_strList.append(str);
}

void PluginPassData::setImage(const QImage &image)
{
    d->_img = QImage(image);
}

QImage &PluginPassData::getImage() const
{
    return d->_img;
}

PluginPassDataPrivate *PluginPassData::createPrivateStruct(const PluginPassData &rhs)
{
    PluginPassDataPrivate* p = new PluginPassDataPrivate;
    p->_strList = QStringList(rhs.strList());
    return p;
}
