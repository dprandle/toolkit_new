#pragma once

#include <Urho3D/Core/Mutex.h>
#include <sstream>
#include <math_utils.h>

#define iout MTDebug_Print(true, false, URHO_LT_INFO)
#define iout_nl MTDebug_Print(true, true, URHO_LT_INFO)
#define iout_ns MTDebug_Print(false, false, URHO_LT_INFO)
#define iout_ns_nl MTDebug_Print(false, true, URHO_LT_INFO)

#define wout MTDebug_Print(true, false, URHO_LT_WARNING)
#define wout_nl MTDebug_Print(true, true, URHO_LT_WARNING)
#define wout_ns MTDebug_Print(false, false, URHO_LT_WARNING)
#define wout_ns_nl MTDebug_Print(false, true, URHO_LT_WARNING)

#define eout MTDebug_Print(true, false, URHO_LT_ERROR)
#define eout_nl MTDebug_Print(true, true, URHO_LT_ERROR)
#define eout_ns MTDebug_Print(false, false, URHO_LT_ERROR)
#define eout_ns_nl MTDebug_Print(false, true, URHO_LT_ERROR)

#define dout MTDebug_Print(true, false, URHO_LT_DEBUG)
#define dout_nl MTDebug_Print(true, true, URHO_LT_DEBUG)
#define dout_ns MTDebug_Print(false, false, URHO_LT_DEBUG)
#define dout_ns_nl MTDebug_Print(false, true, URHO_LT_DEBUG)

#define tout MTDebug_Print(true, false, URHO_LT_TRACE)
#define tout_nl MTDebug_Print(true, true, URHO_LT_TRACE)
#define tout_ns MTDebug_Print(false, false, URHO_LT_TRACE)
#define tout_ns_nl MTDebug_Print(false, true, URHO_LT_TRACE)

enum Urho_Log_Type
{
    URHO_LT_INFO,
    URHO_LT_WARNING,
    URHO_LT_ERROR,
    URHO_LT_DEBUG,
    URHO_LT_TRACE
};

class MTDebug_Print
{
  public:
    MTDebug_Print(bool auto_spacing, bool auto_newline, Urho_Log_Type logtype);
    ~MTDebug_Print();

    MTDebug_Print & operator<<(const Urho3D::String & rhs)
    {
        return *this << rhs.CString();
    }
    
    template<class T>
    MTDebug_Print & operator<<(const T & rhs)
    {
        if (autspc_)
            ss_ << " ";
        ss_ << rhs;
        if (autonln_)
            ss_ << "\n";
        return *this;
    }

  private:
    String str_;
    std::stringstream ss_;
    bool autspc_;
    bool autonln_;
    int lt_;
};
