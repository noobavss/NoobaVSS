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

#ifndef STATPANEL_H
#define STATPANEL_H

#include <QWidget>

namespace Ui {
class StatPanel;
}

class StatPanel : public QWidget
{
    Q_OBJECT

public:
    explicit StatPanel(QWidget *parent = 0);
    ~StatPanel();

    void setDeviceName(const QString& str);
    void setImageBufferLabel(const QString& str);
    void setImageBufferBarValue(int size);
    void setCaptureRateLabel(const QString& rate);
    void setnFramesCapturedLabel(const QString& nFrames);
    void setProcessingRate(const QString& str);
    void setnFramesProcessedLabel(const QString& str);
    void setImageBufferBarSize(int min, int max);

private:
    Ui::StatPanel *ui;
};

#endif // STATPANEL_H
