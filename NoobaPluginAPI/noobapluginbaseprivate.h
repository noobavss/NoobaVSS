#ifndef NOOBAPLUGINBASEPRIVATE_H
#define NOOBAPLUGINBASEPRIVATE_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QScopedPointer>

class Property;
class NoobaPluginAPIBase;

class NoobaPluginAPIBasePrivate
{
public:

    // functions
    explicit NoobaPluginAPIBasePrivate(NoobaPluginAPIBase* parent);
    virtual ~NoobaPluginAPIBasePrivate();
    bool addProperty(Property* property);
    Property* getProperty(const QString& name) const;
    int removeProperty(const QString& name);

    QList<Property* > getPropertyList() const;

    NoobaPluginAPIBase* const      q_ptr;
    Q_DECLARE_PUBLIC(NoobaPluginAPIBase)

private:
    // variables
    QScopedPointer< QMap<QString, Property* > >    _propertyMap;
};

#endif // NOOBAPLUGINBASEPRIVATE_H
