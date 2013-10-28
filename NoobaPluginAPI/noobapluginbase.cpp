#include "noobapluginbase.h"
#include "noobapluginbaseprivate.h"

NoobaPluginAPIBase::NoobaPluginAPIBase()
    :QObject()
    ,_d(new NoobaPluginAPIBasePrivate(this))
{
}
