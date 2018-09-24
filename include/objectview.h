#ifndef OBJECTVIEW_H
#define OBJECTVIEW_H

// Inlcudes
#include <ui_objectview.h>

class ObjectView : public QMainWindow
{
	Q_OBJECT
public:
	ObjectView(QWidget * parent = NULL);
	~ObjectView();

    void init();

	void refresh();

	public slots:

	void onActionNew();
	void onActionDelete();
	void onActionEdit();
    void onSelectionChanged();

private:
    Ui_ObjectView m_ui;
};
#endif
