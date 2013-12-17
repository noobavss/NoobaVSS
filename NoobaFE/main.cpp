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

// Qt
#include <QApplication>
#include <QFile>

// nooba
#include "MainWindow.h"
#include "NoobaEye.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile f(":/Resources/stylesheet.css");
    if(f.open(QFile::ReadOnly))
    {
        QString stylesheet = QLatin1String(f.readAll());
        a.setStyleSheet(stylesheet);
    }
//    QApplication::setStyle(QStyleFactory::create("Fusion"));
    a.setApplicationName(nooba::ProgramName);
    a.setOrganizationName(nooba::Organisation);
    a.setApplicationVersion(QString("%1.%2").arg(nooba::MajorVersion).arg(nooba::MinorVersion));
    a.setWindowIcon(QIcon(":/Resources/Nooba_logo.png"));

    MainWindow w;
    w.show();
    return a.exec();
}
