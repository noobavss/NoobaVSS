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

#ifndef PARAMCONFIGWIND_H
#define PARAMCONFIGWIND_H

#include <QWidget>
#include <QScopedPointer>

#include "noobapluginapi.h"

namespace Ui { class ParamConfigWind; }
//namespace nooba { enum ParamType; }

//class QTreeWidgetItem;
class NoobaPlugin;
class ParamDelegate;
class NoEditDelegate;
class QModelIndex;
class QTreeWidgetItem;

class ParamConfigWind : public QWidget
{
    Q_OBJECT
    
public:
    explicit ParamConfigWind(QWidget *parent = 0);
    ~ParamConfigWind();

public slots:

    void addPlugin(NoobaPlugin* plugin);
    void removePlugin(QString alias);
    void clear();

private slots:

    void onItemChanged(QTreeWidgetItem* item, int  column);

private:

    /**
     * Generic function to add basic datatypes to the tree widget.
     * NOTE:    Type is given as template type to avoid the
     *          inclusion of the NoobaPluginAPI header to this file :)
     */
    template<typename value> void addToTree( const QMap<QString, value >& map, nooba::ParamType type, QTreeWidgetItem* topLevel);

    Ui::ParamConfigWind             *ui;
    int                             PluginPtrRole;
    QScopedPointer<ParamDelegate>   _delegateSP;
    QScopedPointer<NoEditDelegate>  _noEditDelegateSP;

};

#endif // PARAMCONFIGWIND_H
