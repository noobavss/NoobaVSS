#include "port.h"
#include "portprivate.h"

PortPrivate::PortPrivate(const QString &name, Port* parent):
    q_ptr(parent),
    _name(name)
{
}

Port::Port(const QString &name, QObject *parent) :
    QObject(parent),
    d_ptr(new PortPrivate(name, this))
{
}
