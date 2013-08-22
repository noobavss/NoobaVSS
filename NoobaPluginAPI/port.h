#ifndef PORT_H
#define PORT_H

#include <QObject>

class PortPrivate;

class Port : public QObject
{
    Q_OBJECT
public:
    explicit Port(const QString &name, QObject *parent = 0);
    
private:

    PortPrivate* const  d_ptr;
    Q_DECLARE_PRIVATE(Port);
    
};

#endif // PORT_H
