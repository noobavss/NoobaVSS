#ifndef PROPERTYPRIVATE_H
#define PROPERTYPRIVATE_H

#include "property.h"

#include <QObject>

class Property;

class PropertyPrivate
{
public:    

    PropertyPrivate(const QString& name, PropertyType type, Property* parent);

    Property* const q_ptr;
    Q_DECLARE_PUBLIC(Property)

    PropertyType    _type;
    const QString   _name;
    QString         _description;

};

#endif // PROPERTYPRIVATE_H
