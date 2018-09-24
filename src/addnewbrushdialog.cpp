#include <addnewbrushdialog.h>
#include <toolkit.h>

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QStyle>
#include <QMessageBox>
#include <QFileDialog>

AddNewBrushDialog::AddNewBrushDialog(QWidget * parent):
QDialog(parent),	
mScene(new QGraphicsScene(this)),
mNormX(0.0f),
mNormY(0.0f)
{

}

AddNewBrushDialog::~AddNewBrushDialog()
{

}

void AddNewBrushDialog::init()
{
	mUI.setupUi(this);
	mUI.mBrushView->setScene(mScene);
	drawPolygons();
}

void AddNewBrushDialog::drawPolygons()
{
	QRectF rect;
	QSizeF s = mUI.mBrushView->size();

	mNormX = s.width() / 22.0f;
	mNormY = s.height() / 22.0f;

	rect.setTopLeft(QPointF(-s.width()/2.0f, s.height()/2.0f)); rect.setTopRight(QPointF(s.width()/2.0f-2, s.height()/2.0f));
	rect.setBottomRight(QPointF(-s.width() / 2.0f, -s.height() / 2.0f)); rect.setBottomRight(QPointF(s.width() / 2.0f-2, -s.height() / 2.0f));
	mUI.mBrushView->setSceneRect(rect);
	mUI.mBrushView->setStyleSheet("background:transparent");
	
	QPolygonF p;
	p << QPointF(-X_GRID*mNormX, 0.5f*mNormY) << QPointF(0.0f, mNormY) << QPointF(X_GRID * mNormX, 0.5f*mNormY)
		<< QPointF(X_GRID*mNormX, -0.5f*mNormY) << QPointF(0.0f, -mNormY) << QPointF(-X_GRID*mNormX, -0.5f*mNormY) << QPointF(-X_GRID*mNormX, 0.5f*mNormY);
	
	for (int i = -5; i < 6; ++i)
	{
		if (i % 2 != 0)
		{
			for (int j = -5; j < 5; ++j)
			{
				HexTileItem * item = new HexTileItem();
				item->setPolygon(p);
				item->setFlags(QGraphicsItem::ItemIsSelectable);
				item->setBrush(QBrush());
				item->setPos(j * X_GRID * 2 * mNormX + X_GRID*mNormX, i*Y_GRID*mNormY);
				mScene->addItem(item);
			}
		}
		else
		{
			for (int j = -5; j < 6; ++j)
			{
				HexTileItem * item = new HexTileItem();
				item->setPolygon(p);
				item->setFlags(QGraphicsItem::ItemIsSelectable);
				item->setBrush(QBrush());
				item->setPos(j * X_GRID * 2 * mNormX, i*Y_GRID*mNormY);
				mScene->addItem(item);
			}
		}
	}
	
}

void AddNewBrushDialog::setFromEngineSelection()
{
//    uivec2 foc = nse.system<nsselection_system>()->selection_center_entity_id();
//	nsscene * scene =

//	if (scene == NULL)
//		return;

//    nsentity * ent = scene->entity(foc.xy());
//	if (ent == NULL)
//		return;

//	nssel_comp * selComp = ent->get<nssel_comp>();
//	nstform_comp * tComp = ent->get<nstform_comp>();
//	if (selComp == NULL || tComp == NULL)
//		return;

//    ivec3 centerPos = nstile_grid::grid(tComp->wpos(foc.z));

//	auto selIter = selComp->begin();
//	while (selIter != selComp->end())
//	{
//		ivec3 currentPos = nstile_grid::grid(tComp->wpos(*selIter)) - centerPos;

//		float posX = currentPos.x * X_GRID * 2.0f * mNormX;
//		float posY = -currentPos.y * Y_GRID * mNormY;
//		if (currentPos.y % 2 != 0 && centerPos.y % 2 == 0)
//			posX += X_GRID * mNormX;
//		else if (currentPos.y % 2)
//			posX -= X_GRID * mNormX;

//		QGraphicsItem * item = mScene->itemAt(QPointF(posX, posY),QTransform());
//		if (item != NULL)
//			item->setSelected(true);

//		++selIter;
//	}
}

void AddNewBrushDialog::onAddBrush()
{
//	nsstring name = mUI.mObjectNameLE->text().toStdString();
//	nsstring iconFileName = mUI.mIconLE->text().toStdString();
//	nsplugin * act = nse.();

//	if (name.empty())
//	{
//		QMessageBox mb(QMessageBox::Warning, "Object Name Error", "Enter a name for the brush", QMessageBox::NoButton, this);
//		mb.exec();
//		return;
//	}

//	if (act->contains<nsentity>(name))
//	{
//		QMessageBox mb(QMessageBox::Warning, "Object Name Error", "An object already has this name - all game objects (including brushes) must have distinct names.", QMessageBox::NoButton, this);
//		mb.exec();
//		return;
//	}

//	nsentity * tileBrush = act->create<nsentity>(name);

//	if (tileBrush == NULL)
//	{
//		QMessageBox mb(QMessageBox::Warning, "Object Creation Error", "An error has occured in trying to make this brush - check the engine log for details.", QMessageBox::NoButton, this);
//		mb.exec();
//		return;
//	}

//	nstile_brush_comp * tBComp = tileBrush->create<nstile_brush_comp>();
//	nsrender_comp * rComp = tileBrush->create<nsrender_comp>();
//	nssel_comp * sComp = tileBrush->create<nssel_comp>();

//    rComp->set_mesh_id(nse.resource<nsmesh>(ENGINE_PLUG, MESH_FULL_TILE)->full_id());
//    sComp->set_default_sel_color(fvec4(0.0f, 1.0f, 0.0f, 0.8f));

//	auto items = mScene->selectedItems();
//	auto iter = items.begin();
//	while (iter != items.end())
//	{
//		HexTileItem * curItem = (HexTileItem*)*iter;
//		tBComp->add(curItem->getEnginePos());
//		++iter;
//	}

//	if (mUI.mGenIconCB->isChecked())
//        iconFileName = _generateIcon(name,"icons/");

//	if (!iconFileName.empty())
//        tileBrush->set_icon_path(iconFileName);

//    nse.system<nsbuild_system>()->set_tile_brush(tileBrush);
//    bbtk.refresh_views();
//	accept();
}

void AddNewBrushDialog::onIconBrowse()
{
	QString filename = QFileDialog::getOpenFileName(
		this,
		"Choose icon file...",
        "./icons",
		"Image Files (*.png *.jpg *.bmp)"
		);

	if (!filename.isEmpty())
		mUI.mIconLE->setText(filename);
}

void AddNewBrushDialog::onGenerateIconCB(bool pChecked)
{
	mUI.mIconBrowseBtn->setEnabled(!pChecked);
	mUI.mIconLE->setEnabled(!pChecked);
}

QString AddNewBrushDialog::_generateIcon(const QString & pObjName,const QString & pSubdir)
{
	auto items = mScene->items();
	auto iter = items.begin();
	while (iter != items.end())
	{
		if (!(*iter)->isSelected())
			mScene->removeItem(*iter);
		++iter;
	}

	QRect r = mScene->itemsBoundingRect().toRect();
	r.moveCenter(mUI.mBrushView->rect().center() + r.center());
	r.setWidth(r.width() + 5);

	QPixmap pixMap = QPixmap::grabWidget(mUI.mBrushView, r);
	
//    nsstring savePath = nse.active()->manager<nstex_manager>()->res_dir() +
//            nse.active()->manager<nstex_manager>()->local_dir() +
//            pSubdir + pObjName + ".png";

//    nsfile_os::create_dir(savePath);
//    pixMap.save(savePath.c_str());
//    return savePath;
    return QString();
}


HexTileItem::HexTileItem(QGraphicsItem * pParent) :
QGraphicsPolygonItem(pParent)
{
}

HexTileItem::~HexTileItem()
{

}

void HexTileItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	QStyleOptionGraphicsItem myOption(*option);
	myOption.state &= ~QStyle::State_Selected;

	if (isSelected())
		setBrush(QBrush(QColor(255, 200, 200)));
	else
		setBrush(QBrush(QColor(255, 255, 255)));

	if (scenePos() == QPointF(0.0f, 0.0f))
	{
		setSelected(true);
		setBrush(QBrush(QColor(255, 0, 0)));
	}

	QGraphicsPolygonItem::paint(painter, &myOption, widget);
}
