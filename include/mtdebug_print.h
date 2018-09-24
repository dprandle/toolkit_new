#ifndef MTDEBUG_PRINT_H
#define MTDEBUG_PRINT_H

#include <iostream>
#include <QDebug>
class QMutex;

#define dout MTDebug_Print(false, true)
#define dout_plain MTDebug_Print(false, false)
#define dout_ns MTDebug_Print(true, false)
#define dout_nn MTDebug_Print(false, true)


struct mutex_wrapper
{
    mutex_wrapper();
    ~mutex_wrapper();
    void lock();
    void unlock();
    QMutex * mtx;
};

class MTDebug_Print
{
public:

    MTDebug_Print(bool auto_newline, bool auto_spacing);
    ~MTDebug_Print();

    template<class T>
    MTDebug_Print & operator<<(const T & rhs)
    {
        m_mtx.lock();
        dbg << rhs;
        m_mtx.unlock();
        return *this;
    }

private:
    static mutex_wrapper m_mtx;
    QString dbg_string;
    QDebug dbg;
    bool m_auto_newline;
};

#endif // MTDEBUG_PRINT_H
