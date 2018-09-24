#include <listwidgetcust.h>
#include <QMouseEvent>

ListWidgetCust::ListWidgetCust(QWidget * pParent) :
QListWidget(pParent)
{
}

void ListWidgetCust::init()
{
	
}

void ListWidgetCust::mousePressEvent(QMouseEvent * pEvent)
{
	if (!indexAt(pEvent->pos()).isValid())
		clearSelection();
	QListWidget::mousePressEvent(pEvent);
}
