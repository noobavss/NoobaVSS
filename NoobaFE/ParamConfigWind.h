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
