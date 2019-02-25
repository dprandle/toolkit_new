#include <toolkit.h>
#include <ui_toolkit.h>
#include <mtdebug_print.h>
#include <Urho3D/IO/Log.h>
#include <console.h>

using namespace Urho3D;

Mutex MTDebug_Print::mtx_;

MTDebug_Print::MTDebug_Print(bool auto_spacing, bool auto_newline, Urho_Log_Type logtype)
    : str_(), ss_(), autspc_(auto_spacing), autonln_(auto_newline), lt_(logtype)
{}

MTDebug_Print::~MTDebug_Print()
{
    mtx_.Acquire();
    String msg(ss_.str().c_str());
    mtx_.Release();
    switch (lt_)
    {
    case (URHO_LT_INFO):
        URHO3D_LOGINFO(msg);
        break;
    case (URHO_LT_WARNING):
        URHO3D_LOGWARNING(msg);
        break;
    case (URHO_LT_ERROR):
        URHO3D_LOGERROR(msg);
        break;
    case (URHO_LT_DEBUG):
        URHO3D_LOGDEBUG(msg);
        break;
    case (URHO_LT_TRACE):
        URHO3D_LOGTRACE(msg);
        break;
    }
}
