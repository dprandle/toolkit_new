#include "mtdebug_print.h"
#include <QMutex>
#include <QString>

mutex_wrapper MTDebug_Print::m_mtx;

mutex_wrapper::mutex_wrapper():
    mtx(new QMutex)
{}

mutex_wrapper::~mutex_wrapper()
{
    delete mtx;
}

void mutex_wrapper::lock()
{
    mtx->lock();
}

void mutex_wrapper::unlock()
{
    mtx->unlock();
}

MTDebug_Print::MTDebug_Print(bool auto_newline, bool auto_spacing):
    dbg_string(),
    dbg(&dbg_string),
    m_auto_newline(auto_newline)
{
    if (!auto_spacing)
        dbg.nospace();
}

MTDebug_Print::~MTDebug_Print()
{
    m_mtx.lock();

    if (m_auto_newline)
        std::cout << dbg_string.toStdString() << std::endl;
    else
        std::cout << dbg_string.toStdString() << std::flush;

	m_mtx.unlock();

	#ifdef DEBUG_VERSION
	if (m_auto_newline)
		ubermail_inst.console()->print(dbg_string + "\n");
	else
		ubermail_inst.console()->print(dbg_string);
	#endif

}
