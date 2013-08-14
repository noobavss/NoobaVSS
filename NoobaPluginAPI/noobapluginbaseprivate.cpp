#include <QMap>

#include "noobapluginbaseprivate.h"
#include "noobapluginbase.h"
#include "property.h"


NoobaPluginAPIBasePrivate::NoobaPluginAPIBasePrivate(NoobaPluginAPIBase* parent)
    :q_ptr(parent)
{
}

NoobaPluginAPIBasePrivate::~NoobaPluginAPIBasePrivate()
{

}

bool NoobaPluginAPIBasePrivate::addProperty(Property *property)
{
    _propertyMap.insert(property->getName(), property);
    return true;
}

Property *NoobaPluginAPIBasePrivate::getProperty(const QString &name) const
{
    return _propertyMap.value(name, NULL);
}

int NoobaPluginAPIBasePrivate::removeProperty(const QString &name)
{
    return _propertyMap.remove(name);
}

QList<Property *> NoobaPluginAPIBasePrivate::getPropertyList() const
{
    return _propertyMap.values();
}
