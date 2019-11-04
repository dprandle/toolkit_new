#include <custom_dock_widget.h>
#include <mtdebug_print.h>
#include <QWindowStateChangeEvent>

Custom_Dock_Widget::Custom_Dock_Widget(QWidget *parent):
QDockWidget(parent),
floating_(false)
{}

Custom_Dock_Widget::~Custom_Dock_Widget()
{}

bool Custom_Dock_Widget::manually_floating()
{
    return floating_;
}

void Custom_Dock_Widget::changeEvent(QEvent * e)
{
    floating_ = isFloating();
    QDockWidget::changeEvent(e);
}
