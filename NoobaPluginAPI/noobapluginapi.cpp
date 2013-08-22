#include "noobapluginapi.h"

struct ProcParamsPrivate
{
    ProcParamsPrivate():
        _isError(false),
        _frameId(-1),
        _frameRate(-1) {}

    // variables
    // ---------------------------------------------------------------
    // NOTE: UPDATE CreatePrivateStruct(...) function on addition of
    //       New variables. Copy and assignment constructors use it.
    // ---------------------------------------------------------------
    bool        _isError;   // frame process status, successful or not
    int         _frameId;   // unique frameId for the video
    double      _frameRate; //
    QString     _errMsg;    // error details
};


ProcParams::ProcParams():
    d(new ProcParamsPrivate())
{
}

ProcParams::ProcParams(const ProcParams &rhs)
{
    d = createPrivateStruct(rhs);
}

ProcParams &ProcParams::operator =(const ProcParams &rhs)
{
    if(this == &rhs)
        return *this;

    d = createPrivateStruct(rhs);

    return *this;
}

ProcParams::~ProcParams()
{
    delete d;
}

void ProcParams::setFrameId(int id)
{
    d->_frameId = id;
}

int ProcParams::frameId() const
{
    return d->_frameId;
}

void ProcParams::setFrameRate(double rate)
{
    d->_frameRate = rate;
}

double ProcParams::frameRate() const
{
    return d->_frameRate;
}

void ProcParams::setErrorState(bool isError)
{
    d->_isError = isError;
}

bool ProcParams::isError() const
{
    return d->_isError;
}

void ProcParams::setErrorMsg(const QString &errMsg)
{
    d->_errMsg = errMsg;
}

QString ProcParams::errMsg() const
{
    return d->_errMsg;
}

ProcParamsPrivate *ProcParams::createPrivateStruct(const ProcParams &rhs)
{
    ProcParamsPrivate *t = new ProcParamsPrivate;
    t->_errMsg = rhs.errMsg();
    t->_frameId = rhs.frameId();
    t->_frameRate = rhs.frameRate();
    t->_isError = rhs.isError();

    return t;
}

//////////////////////////////////////////////////////////////
// PluginInfo class and its private data structure

struct PluginInfoPrivate
{
    PluginInfoPrivate(const QString& name, const int majorVersion, const int minorVersion,
                      const QString& description, const QString& author):
        _name(name),
        _description(description),
        _author(author),
        _majorVersion(majorVersion),
        _minorVersion(minorVersion) {}

    // variables
    // ---------------------------------------------------------------
    // NOTE: UPDATE CreatePrivateStruct(...) function on addition of
    //       New variables. Copy and assignment constructors use it.
    // ---------------------------------------------------------------

    QString     _name;          // name of the plugin
    QString     _description;   // small explanation of the plugin
    QString     _author;        // creator(s) of the plugin

   /* version numbers of a processing class has a major version and a minor version
    * eg: 1.5 , here 1 is the major version and 5 is the minor version
    */
    int         _majorVersion; // major version of plugin, different from API version
    int         _minorVersion; // minor version of plugin, different from API version
};


PluginInfo::PluginInfo(const QString &name, const int majorVersion,
                       const int minorVersion, const QString &description, const QString &author)
    : d(new PluginInfoPrivate(name,majorVersion, minorVersion, description, author))
{
}

PluginInfo::PluginInfo(const PluginInfo &rhs)
{
    d = createPrivateStruct(rhs);
}

PluginInfo &PluginInfo::operator =(const PluginInfo &rhs)
{
    if(this == &rhs)
        return *this;

    d = createPrivateStruct(rhs);

    return *this;
}

PluginInfo::~PluginInfo()
{
    delete d;
}

QString PluginInfo::name() const
{
    return d->_name;
}

QString PluginInfo::description() const
{
    return d->_description;
}

QString PluginInfo::author() const
{
    return d->_author;
}

int PluginInfo::majorVersion() const
{
    return d->_majorVersion;
}

int PluginInfo::minorVersion() const
{
    return d->_minorVersion;
}

PluginInfoPrivate* PluginInfo::createPrivateStruct(const PluginInfo &rhs)
{
    return new PluginInfoPrivate( rhs.name(), rhs.majorVersion(), rhs.minorVersion(),
                rhs.description(), rhs.author());
}


//////////////////////////////////////////////////////////////////////////////
// NoobaPluginAPI class

// private data structure for NoobaPluginAPI class
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

void NoobaPluginAPI::setErrMsg(const QString &errMsg)
{
    d->_errMsg = errMsg;
}

QString NoobaPluginAPI::errMsg() const
{
    return d->_errMsg;
}
