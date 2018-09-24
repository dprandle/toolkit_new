#ifndef ADDNEWBRUSHDIALOG_H
#define ADDNEWBRUSHDIALOG_H

// Inlcudes
#include <ui_addnewbrushdialog.h>
#include <qgraphicsitem.h>

#define X_GRID 0.86f
#define Y_GRID 1.49f
#define Z_GRID 0.45f

class Toolkit;
class QGraphicsScene;

class AddNewBrushDialog : public QDialog
{
	Q_OBJECT
public:
	AddNewBrushDialog(QWidget * parent = NULL);
	~AddNewBrushDialog();

    void init();
	void drawPolygons();
	void setFromEngineSelection();

	public slots:

	void onAddBrush();
	void onIconBrowse();
	void onGenerateIconCB(bool);

private:
	QString _generateIcon(const QString & pObjName, const QString & pSubdir="");

	Ui_AddNewBrushDialog mUI;
	QGraphicsScene * mScene;
	float mNormX;
	float mNormY;
};


class HexTileItem : public QGraphicsPolygonItem
{
public:
	HexTileItem(QGraphicsItem * pParent=NULL);
	~HexTileItem();

	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
};

#endif
