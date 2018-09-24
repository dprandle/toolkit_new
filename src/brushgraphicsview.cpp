#include <brushgraphicsview.h>
#include <QGraphicsScene>
#include <QEvent>
#include <QGraphicsItem>
#include <QKeyEvent>


BrushGraphicsView::BrushGraphicsView(QWidget * parent):
QGraphicsView(parent),
mCTRLHeld(false)
{

}

BrushGraphicsView::~BrushGraphicsView()
{

}


void BrushGraphicsView::keyPressEvent(QKeyEvent * keyEvent)
{
	if (keyEvent->isAutoRepeat())
	{
		QGraphicsView::keyPressEvent(keyEvent);
		return;
	}

	switch (keyEvent->key())
	{
	case (Qt::Key_Control) :
		mCTRLHeld = true;
		break;
	}
	QGraphicsView::keyPressEvent(keyEvent);
}

void BrushGraphicsView::keyReleaseEvent(QKeyEvent * keyEvent)
{
	if (keyEvent->isAutoRepeat())
	{
		QGraphicsView::keyReleaseEvent(keyEvent);
		return;
	}



	switch (keyEvent->key())
	{
	case (Qt::Key_Control) :
		mCTRLHeld = false;
		break;
	}
	QGraphicsView::keyReleaseEvent(keyEvent);
}

void BrushGraphicsView::mouseMoveEvent(QMouseEvent * mevent)
{
	QGraphicsItem * item = itemAt(mevent->pos());
	if (item != NULL)
		item->setSelected(true);
}

void BrushGraphicsView::mousePressEvent(QMouseEvent * mevent)
{
	if (!mCTRLHeld)
		scene()->clearSelection();

	QGraphicsItem * item = itemAt(mevent->pos());
	if (item != NULL)
		item->setSelected(!item->isSelected());
	else
		scene()->clearSelection();
}
