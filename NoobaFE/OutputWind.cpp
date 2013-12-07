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

#include "OutputWind.h"
#include "ui_OutputWind.h"

OutputWind::OutputWind(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputWind)
{
    ui->setupUi(this);
}

OutputWind::~OutputWind()
{
    delete ui;
}

void OutputWind::onDebugMsg(const QString &msg)
{
    ui->textEdit->append(msg);
}
