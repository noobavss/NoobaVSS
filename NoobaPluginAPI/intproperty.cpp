#include "intproperty.h"
#include "property.h"
#include "propertyprivate.h"

class IntPropertyPrivate: public PropertyPrivate
{
public:
    IntPropertyPrivate(const QString& name, IntProperty* parent):
        PropertyPrivate(name, intProperty, parent),
        _value(0),
        _max(100),
        _min(0) {}

    int _value;
    int _max;
    int _min;
};

IntProperty::IntProperty(const QString& name):
    Property(name, intProperty, this)
{
}

void IntProperty::setValue(int val)
{
    Q_D(IntProperty);
    d->_value = val;
}

bool IntProperty::setMaxValue(int val)
{
    Q_D(IntProperty);
    d->_max = val;
    return true;
}

bool IntProperty::setMinValue(int val)
{
    Q_D(IntProperty);
    d->_min = val;
    return true;
}

int IntProperty::getValue() const
{
    Q_D(const IntProperty);
    return d->_value;
}

int IntProperty::getMaxValue() const
{
    Q_D(const IntProperty);
    return d->_max;
}

int IntProperty::getMinValue() const
{
    Q_D(const IntProperty);
    return d->_min;
}

IntProperty::IntProperty(IntPropertyPrivate &d)
    :Property(d)
{
}
