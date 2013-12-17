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

#ifndef PROCPARAMS_H
#define PROCPARAMS_H

#include "nooba.h"
//Qt
#include <QMetaType>

struct ProcParamsPrivate;

/*!
 * \class ProcParams
 * \brief Structure that uses to pass frame relevant data from the Frontend application to
 *        base plugin.
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

    /*!
     * \brief currentVideoState gives the current video state
     * \return
     */
    nooba::VideoState currentVideoState() const;

    /*!
     * \brief setVideoState Currently Nothing happens by calling this. Only changes the internal variable.
     *                      No effect on the actual state.
     * \param state
     * \return bool setting the video state was sucessful or not
     */
    bool setVidState(nooba::VideoState state);

private:

    ProcParamsPrivate* createPrivateStruct(const ProcParams& rhs);
    ProcParamsPrivate*   d;  // d pointer

};

Q_DECLARE_METATYPE(ProcParams*)

#endif // PROCPARAMS_H
