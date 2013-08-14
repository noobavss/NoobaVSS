#include "noobapluginbase.h"
#include "noobapluginbaseprivate.h"

NoobaPluginAPIBase::NoobaPluginAPIBase()
    :QObject()
    ,_d(new NoobaPluginAPIBasePrivate(this))
{
}


bool NoobaPluginAPIBase::addProperty(Property *property)
{
    Q_D(NoobaPluginAPIBase);
    property->setParent(this);
    return d->addProperty(property);
}

bool NoobaPluginAPIBase::removeProperty(const QString &name)
{
    Q_D(NoobaPluginAPIBase);
    int c = d->removeProperty(name);
    bool ok;
    (c > 0) ? ok = true: ok = false;
    return ok;
}

Property *NoobaPluginAPIBase::getProperty(const QString &name)
{
    Q_D(NoobaPluginAPIBase);
    return d->getProperty(name);
}

QList<Property *> NoobaPluginAPIBase::getPropertyList()
{
    Q_D(NoobaPluginAPIBase);
    return d->getPropertyList();
}
