#ifndef TILEVIEW_H
#define TILEVIEW_H

#include <ui_tileview.h>

class TileView : public QMainWindow
{
	Q_OBJECT
public:
	TileView(QWidget * parent = NULL);
	~TileView();

    void init();
	void reset();
	void refresh();
    Ui_TileView m_ui;

public slots:

	void onActionNew();
	void onActionDelete();
	void onActionEdit();
	void onSelectionChanged();

};
#endif
