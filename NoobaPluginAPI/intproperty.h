#ifndef INTPROPERTY_H
#define INTPROPERTY_H

#include "property.h"

class IntPropertyPrivate;

class IntProperty: public Property
{    

    Q_OBJECT

public:

    IntProperty(const QString &name);
    void setValue(int val);
    bool setMaxValue(int val);
    bool setMinValue(int val);

    int getValue() const;
    int getMaxValue() const;
    int getMinValue() const;

protected:

    IntProperty(IntPropertyPrivate &d);
    Q_DECLARE_PRIVATE(IntProperty);
};

#endif // INTPROPERTY_H
