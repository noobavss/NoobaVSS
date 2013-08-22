#include <QMap>

#include "noobapluginbaseprivate.h"
#include "noobapluginbase.h"
#include "property.h"


NoobaPluginAPIBasePrivate::NoobaPluginAPIBasePrivate(NoobaPluginAPIBase* parent)
    :q_ptr(parent),
      _propertyMap(new QMap< QString, Property* >())
{
}

NoobaPluginAPIBasePrivate::~NoobaPluginAPIBasePrivate()
{

}

bool NoobaPluginAPIBasePrivate::addProperty(Property *property)
{
    _propertyMap.data()->insert(property->getName(), property);
    return true;
}

Property *NoobaPluginAPIBasePrivate::getProperty(const QString &name) const
{
    return _propertyMap.data()->value(name, NULL);
}

int NoobaPluginAPIBasePrivate::removeProperty(const QString &name)
{
    return _propertyMap.data()->remove(name);
}

QList<Property *> NoobaPluginAPIBasePrivate::getPropertyList() const
{
    return _propertyMap.data()->values();
}
