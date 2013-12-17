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

#include "procparams.h"

//Qt
#include <QString>

struct ProcParamsPrivate
{
    ProcParamsPrivate():
        _isError(false),
        _currentState(nooba::StoppedState),
        _frameId(-1),
        _frameRate(-1) {}

    // variables
    // ---------------------------------------------------------------
    // NOTE: UPDATE CreatePrivateStruct(...) function on addition of
    //       New variables. Copy and assignment constructors use it.
    // ---------------------------------------------------------------
    bool        _isError;   // frame process status, successful or not
    nooba::VideoState  _currentState;
    int         _frameId;   // unique frameId for the video
    double      _frameRate; //
    QString     _errMsg;    // error details
};

ProcParams::ProcParams():
    d(new ProcParamsPrivate())
{
}

ProcParams::ProcParams(const ProcParams &rhs)
{
    d = createPrivateStruct(rhs);
}

ProcParams &ProcParams::operator =(const ProcParams &rhs)
{
    if(this == &rhs)
        return *this;

    d = createPrivateStruct(rhs);

    return *this;
}

ProcParams::~ProcParams()
{
    delete d;
}

void ProcParams::setFrameId(int id)
{
    d->_frameId = id;
}

int ProcParams::frameId() const
{
    return d->_frameId;
}

void ProcParams::setFrameRate(double rate)
{
    d->_frameRate = rate;
}

double ProcParams::frameRate() const
{
    return d->_frameRate;
}

void ProcParams::setErrorState(bool isError)
{
    d->_isError = isError;
}

bool ProcParams::isError() const
{
    return d->_isError;
}

void ProcParams::setErrorMsg(const QString &errMsg)
{
    d->_errMsg = errMsg;
}

QString ProcParams::errMsg() const
{
    return d->_errMsg;
}

nooba::VideoState ProcParams::currentVideoState() const
{
    return d->_currentState;
}

bool ProcParams::setVidState(nooba::VideoState state)
{
    d->_currentState = state;
    return false;   // currently this will not set the actual state. only the proc param variable
}

ProcParamsPrivate *ProcParams::createPrivateStruct(const ProcParams &rhs)
{
    ProcParamsPrivate *t = new ProcParamsPrivate;
    t->_errMsg = rhs.errMsg();
    t->_frameId = rhs.frameId();
    t->_frameRate = rhs.frameRate();
    t->_isError = rhs.isError();
    t->_currentState = rhs.currentVideoState();
    return t;
}


