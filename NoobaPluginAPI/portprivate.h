#ifndef PORTPRIVATE_H
#define PORTPRIVATE_H

#include <QObject>
#include <QVariant>

class Port;

class PortPrivate
{
public:

    PortPrivate(const QString &name, Port* parent);

    Port* const     q_ptr;
    Q_DECLARE_PUBLIC(Port);

    QString     _name;
    QVariant    _userData;

};

#endif // PORTPRIVATE_H
