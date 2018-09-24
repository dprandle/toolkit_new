#ifndef BRUSHMENUWIDGET_H
#define BRUSHMENUWIDGET_H

// Inlcudes
#include <ui_brushmenuwidget.h>

class QToolBar;

class BrushMenuWidget : public QWidget
{
	
	Q_OBJECT

public:

	BrushMenuWidget(QWidget * parent = NULL);
	~BrushMenuWidget();

    void init();
	void setupLW();

public slots:
	void onDeleteBrush();
	void onEditBrush();
	void onDoubleClick();
	void onNewBrush();
    void onSelectionChanged();

signals:
	void brushChange(QListWidgetItem*);
	void brushDoubleClick();

private:
    QToolBar * mTB;
	Ui_BrushMenuWidget mUI;
};
#endif
