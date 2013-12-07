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

#ifndef OUTPUTWIND_H
#define OUTPUTWIND_H

#include <QWidget>

namespace Ui {
class OutputWind;
}

class OutputWind : public QWidget
{
    Q_OBJECT
    
public:
    explicit OutputWind(QWidget *parent = 0);
    ~OutputWind();

public slots:

    void onDebugMsg(const QString& msg);
    
private:
    Ui::OutputWind *ui;
};

#endif // OUTPUTWIND_H
