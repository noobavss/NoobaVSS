#ifndef NOOBAPLUGINBASE_H
#define NOOBAPLUGINBASE_H

#include <QObject>

#define API_MAJOR_VERSION   0
#define API_MINOR_VERSION   4

class NoobaPluginAPIBasePrivate;

class NoobaPluginAPIBase: public QObject
{
    Q_OBJECT

public:

    static int APIMajorVersion() { return API_MAJOR_VERSION; }
    static int APIMinorVersion() { return API_MINOR_VERSION; }

protected:

    NoobaPluginAPIBase();
    virtual ~NoobaPluginAPIBase() {}

private:

    NoobaPluginAPIBasePrivate* const _d;       // d pointer, refer http://qt-project.org/wiki/Dpointer
    Q_DECLARE_PRIVATE(NoobaPluginAPIBase);
};

#endif // NOOBAPLUGINBASE_H
