#include "noobapluginapi.h"

///////////////////////////////////////////////////
/// \brief The NoobaPluginAPIPrivate struct
/// private data structure for NoobaPluginAPI class
///
struct NoobaPluginAPIPrivate
{
    NoobaPluginAPIPrivate(){}
    QString     _errMsg;
};

NoobaPluginAPI::NoobaPluginAPI()
    : NoobaPluginAPIBase(),
      d(new NoobaPluginAPIPrivate)
{
}



