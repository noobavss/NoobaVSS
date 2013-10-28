#ifndef NOOBAPLUGINBASEPRIVATE_H
#define NOOBAPLUGINBASEPRIVATE_H

#include <QObject>

class Property;
class NoobaPluginAPIBase;

class NoobaPluginAPIBasePrivate
{
public:

    // functions
    explicit NoobaPluginAPIBasePrivate(NoobaPluginAPIBase* parent);
    virtual ~NoobaPluginAPIBasePrivate();

    NoobaPluginAPIBase* const      q_ptr;
    Q_DECLARE_PUBLIC(NoobaPluginAPIBase)

};

#endif // NOOBAPLUGINBASEPRIVATE_H
