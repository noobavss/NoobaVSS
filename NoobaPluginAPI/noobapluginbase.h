#ifndef NOOBAPLUGINBASE_H
#define NOOBAPLUGINBASE_H

#include <QObject>

#define API_MAJOR_VERSION   0
#define API_MINOR_VERSION   3

class NoobaPluginAPIBasePrivate;

class NoobaPluginAPIBase: public QObject
{
    Q_OBJECT

public:

    static int APIMajorVersion() { return API_MAJOR_VERSION; }
    static int APIMinorVersion() { return API_MINOR_VERSION; }

//    Property* getProperty(const QString& name);
//    QList<Property* > getPropertyList();

protected:

    NoobaPluginAPIBase();
    virtual ~NoobaPluginAPIBase() {}
//    /**
//     * @brief adds new proprty. Takes ownership of the property object
//     * @param property
//     * @return true if successful
//     */
//    bool addProperty(Property* property);
//    bool removeProperty(const QString& name);

private:

    NoobaPluginAPIBasePrivate* const _d;       // d pointer, refer http://qt-project.org/wiki/Dpointer
    Q_DECLARE_PRIVATE(NoobaPluginAPIBase);
};

#endif // NOOBAPLUGINBASE_H
