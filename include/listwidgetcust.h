#include <qlistwidget.h>

#ifndef CUSTOMLIST_H
#define CUSTOMLIST_H
class Toolkit;

class ListWidgetCust : public QListWidget
{
	Q_OBJECT
public:
	ListWidgetCust(QWidget * pParent = NULL);

    void init();

protected:

	void mousePressEvent(QMouseEvent * pEvent);

};
#endif
