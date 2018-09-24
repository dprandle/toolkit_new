#ifndef CLICKABLEFRAME_H
#define CLICKABLEFRAME_H

#include <qframe.h>

class ClickableFrame : public QFrame
{
	Q_OBJECT
public:
	explicit ClickableFrame(QWidget * parent = 0);
	~ClickableFrame();

signals:
	void clicked();

protected:
	void mousePressEvent(QMouseEvent * event);
};


#endif