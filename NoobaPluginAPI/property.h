#ifndef PROPERTY_H
#define PROPERTY_H

#include <QObject>

class PropertyPrivate;

enum PropertyType
{
    intProperty,
    doubleProperty,
    stringProperty,
    multiValProperty
};

class Property : public QObject
{
    Q_OBJECT

public:

    PropertyType getType() const;
    QString getName() const;
    QString getDescription() const;
    void setDescription(const QString& description);

signals:

    void propertyChanged(const Property& property);

protected:

    explicit Property(PropertyPrivate& d);      // allow subclasses to initialize with their own concrete Private

    explicit Property(const QString& name, PropertyType type, QObject *parent = 0);        // can't create a property object
    Property(const Property& rhs);                                      // can't copy object
    Property& operator=(const Property& rhs);                           // can't assign object

    PropertyPrivate* const d_ptr;                                       // d_ptr
    Q_DECLARE_PRIVATE(Property)
    
};
#endif // PROPERTY_H
