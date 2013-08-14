#include "property.h"
#include "propertyprivate.h"


PropertyPrivate::PropertyPrivate(const QString &name, PropertyType type, Property *parent)
    :q_ptr(parent)
    ,_type(type)
    ,_name(name)
{
}

Property::Property(const QString &name,PropertyType type, QObject *parent) :
    QObject(parent),
    d_ptr(new PropertyPrivate(name, type, this))
{
}

PropertyType Property::getType() const
{
    return d_ptr->_type;
}

QString Property::getName() const
{
    return d_ptr->_name;
}

QString Property::getDescription() const
{
    return d_ptr->_description;
}

void Property::setDescription(const QString &description)
{
    d_ptr->_description = description;
}

Property::Property(PropertyPrivate &d)
    : d_ptr(&d)
{
}
