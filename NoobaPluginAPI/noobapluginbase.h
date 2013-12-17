#ifndef NOOBAPLUGINBASE_H
#define NOOBAPLUGINBASE_H

#include <QObject>



class NoobaPluginAPIBasePrivate;

class NoobaPluginAPIBase: public QObject
{
    Q_OBJECT

public:



protected:

    NoobaPluginAPIBase();
    virtual ~NoobaPluginAPIBase() {}

private:

    NoobaPluginAPIBasePrivate* const _d;       // d pointer, refer http://qt-project.org/wiki/Dpointer
    Q_DECLARE_PRIVATE(NoobaPluginAPIBase);
};

Q_DECLARE_INTERFACE(NoobaPluginAPIBase, "NoobaVSS.NoobaPluginAPIBase/" )
#endif // NOOBAPLUGINBASE_H
