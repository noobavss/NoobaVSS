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

#ifndef PATHCHOOSER_H
#define PATHCHOOSER_H

#include "noobapluginapi.h"

#include <QFileDialog>
#include <QWidget>

namespace Ui {class PathLineEdit;}

class PathLineEdit : public QWidget
{
	
	Q_OBJECT

public:

    PathLineEdit(QWidget *parent = 0);
    ~PathLineEdit();
    void setText(const QString& _path);
    QString getText() const;
    void setFileMode(nooba::PathType pathType);
    void setFilter(const QString& filter);

private slots:

	void on_browseButtonClicked();

protected:

	 void focusInEvent ( QFocusEvent * event );
	 void focusOutEvent( QFocusEvent * event);

private:

    Ui::PathLineEdit        *ui;
    nooba::PathType         _pathType;
    QString                 _path;
    QString                 _filter;

};

#endif // PATHCHOOSER_H
