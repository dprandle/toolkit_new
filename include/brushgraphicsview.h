#ifndef BRUSHGRAPHICSVIEW_H
#define BRUSHGRAPHICSVIEW_H

// Inlcudes
#include <QGraphicsView>

class BrushGraphicsView : public QGraphicsView
{

	Q_OBJECT

public:

	BrushGraphicsView(QWidget * parent = NULL);
	~BrushGraphicsView();

protected:
	void keyPressEvent(QKeyEvent * keyEvent);
	void keyReleaseEvent(QKeyEvent * keyEvent);
	void mouseMoveEvent(QMouseEvent * mevent);
	void mousePressEvent(QMouseEvent * mevent);


private:
	bool mCTRLHeld;
};
#endif
