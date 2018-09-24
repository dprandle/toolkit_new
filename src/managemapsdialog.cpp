#include <QTreeWidget>
#include <managemapsdialog.h>
#include <newmapdialog.h>
#include <QMessageBox>
#include <toolkit.h>
#include <ui_editmapdialog.h>
#include <QColorDialog>

ManageMapsDialog::ManageMapsDialog(QWidget * parent):
QDialog(parent),
mEditD(this),
mNewMapD(new NewMapDialog(parent))
{

}

ManageMapsDialog::~ManageMapsDialog()
{
	delete mNewMapD;
}

/*!
Initialize the dialog table widget with scenes int the current plugin
*/
void ManageMapsDialog::init()
{
	mUI.setupUi(this);
    mNewMapD->init();
	_rescanPluginMaps();

	mEditMapUI.setupUi(&mEditD);
	connect(mEditMapUI.mColorBtn_2, SIGNAL(clicked()), this, SLOT(onEditColClick()));
}

/*!
When a item is selected we want to display the maps information in the appropriate fields
*/
void ManageMapsDialog::onMapItemChanged(QTreeWidgetItem* pItem, int pColumn)
{
	if (pItem == NULL) // Dont do anything if NULL item clicked.. not sure this can actually even happen
		return;

	if (pItem->checkState(0) == Qt::Unchecked)
		pItem->setCheckState(pColumn, Qt::Checked);

	mUI.mMapsTW->blockSignals(true);
	for (int index = 0; index < mUI.mMapsTW->topLevelItemCount(); ++index)
	{
		auto item = mUI.mMapsTW->topLevelItem(index);
		if (pItem != item)
		{
			item->setCheckState(pColumn, Qt::Unchecked);
		}
	}
	mUI.mMapsTW->blockSignals(false);
}

void ManageMapsDialog::onMapItemPressed(QTreeWidgetItem* pItem, int pColumn)
{
//	if (pItem == NULL) // Dont do anything if NULL item clicked.. not sure this can actually even happen
//		return;

//	nsplugin * aplg = nse.active();

//	nsscene * mMap = aplg->get<nsscene>(pItem->text(pColumn).toStdString()); // Get the map that was clicked by looking up the name

//	if (mMap == NULL) // Again do nothing if map not found
//		return;

//	// Now go through and start setting the fields
//	mUI.mCreatorLE->setText(mMap->creator().c_str());
//    mUI.mNotesTextBox->setPlainText(mMap->notes().c_str());
//    mUI.mMaxPlayersLbl->setText(QString::number(mMap->max_players()));

//	pItem->setCheckState(pColumn, Qt::Checked);
}

void ManageMapsDialog::onNewMap()
{
	if (mNewMapD->exec() == QDialog::Accepted)
	{
		_rescanPluginMaps();
	}
}

void ManageMapsDialog::onDeleteMap()
{
//	auto items = mUI.mMapsTW->selectedItems();
//	if (items.isEmpty())
//		return;

//	QString mapNameQt = items.first()->text(0);
//	QMessageBox mb(QMessageBox::Warning, "Delete", "Do you want to remove " + mapNameQt+" permanently from the hard drive ? ", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this);
//	int ret = mb.exec();
//	if (ret != QMessageBox::Cancel)
//	{
//		nsstring mapName = mapNameQt.toStdString();
//		nsscene * map = nse.active()->get<nsscene>(mapName);

//		bool remFromFile = (ret == QMessageBox::Yes);
//		if (remFromFile)
//			nse.active()->del(map);
//		else
//			nse.active()->destroy(map);

//		_rescanPluginMaps();
//	}
}

void ManageMapsDialog::onEditMap()
{
//	auto items = mUI.mMapsTW->selectedItems();
//	if (items.isEmpty())
//		return;

//	nsstring mapName = items.first()->text(0).toStdString();
//	nsscene * map = nse.active()->get<nsscene>(mapName);

//	mEditMapUI.mMapNameLE->setText(mapName.c_str());
//	mEditMapUI.mCreatorLE->setText(map->creator().c_str());
//    mEditMapUI.mMaxPlayersSB_2->setValue(map->max_players());
//    mEditMapUI.mSceneNotesPTE_2->setPlainText(map->notes().c_str());

//    fvec4 col = map->bg_color();
//	prevC.setRgbF(col.x, col.y, col.z);

//	QString rgbcol = "rgb(" + QString::number(prevC.red()) + "," + QString::number(prevC.green()) + "," + QString::number(prevC.blue()) + ");";
//	mEditMapUI.mColorBtn_2->setStyleSheet(
//		"QFrame {\
//		background-color: " + rgbcol +
//		"border: none\
//		}\
//		QFrame:hover\
//		{\
//		background-color: rgb(200,200,200);\
//		}");

//	if (mEditD.exec() == QDialog::Accepted)
//	{
//		nsstring name = mEditMapUI.mMapNameLE->text().toStdString();
//		if (map->name() != name)
//			map->rename(name);

//		nsstring creator = mEditMapUI.mCreatorLE->text().toStdString();
//		if (map->creator() != creator)
//            map->set_creator(creator);

//        if (map->max_players() != mEditMapUI.mMaxPlayersSB_2->value())
//            map->set_max_players(mEditMapUI.mMaxPlayersSB_2->value());

//		nsstring notes = mEditMapUI.mSceneNotesPTE_2->toPlainText().toStdString();
//		if (map->notes() != notes)
//            map->set_notes(notes);

//        fvec4 col(map->bg_color());
//        fvec4 nCol(prevC.redF(), prevC.greenF(), prevC.blueF(),1.0f);
//		if (col != nCol)
//            map->set_bg_color(nCol);

//		_rescanPluginMaps();
//	}
}

void ManageMapsDialog::onEditColClick()
{
	
	QColor c = QColorDialog::getColor(prevC, this);
	if (c.isValid())
		prevC = c;

	QString rgbcol = "rgb(" + QString::number(prevC.red()) + "," + QString::number(prevC.green()) + "," + QString::number(prevC.blue()) + ");";
	mEditMapUI.mColorBtn_2->setStyleSheet(
		"QFrame {\
		background-color: " + rgbcol +
		"border: none\
		}\
		QFrame:hover\
		{\
		background-color: rgb(200,200,200);\
		}");
}

/*!
Helper function to scan in the maps that are included in the current plugin
*/
void ManageMapsDialog::_rescanPluginMaps()
{
//	mUI.mMapsTW->clear();
//	nsscene_manager * sceneManager = nse.active()->manager<nsscene_manager>();
//	auto sceneBeg = sceneManager->begin();
//	while (sceneBeg != sceneManager->end())
//	{
//		nsscene * scene = (nsscene*)sceneBeg->second;
//        if (scene->plugin_id() == nse.active()->id())
//		{
//			QTreeWidgetItem * item = new QTreeWidgetItem();
//			item->setText(0, sceneBeg->second->name().c_str());

//			if (sceneManager->current() == scene)
//				item->setCheckState(0, Qt::Checked);
//			else
//				item->setCheckState(0, Qt::Unchecked);

//			mUI.mMapsTW->addTopLevelItem(item);
//		}
//		++sceneBeg;
//	}
}

void ManageMapsDialog::onLoad()
{
//    bbtk.on_actionSelect_triggered(true);
//	nse.system<nsselection_system>()->clear();

//	for (int32 index = 0; index < mUI.mMapsTW->topLevelItemCount(); ++index)
//	{
//		auto item = mUI.mMapsTW->topLevelItem(index);
//		if (item->checkState(0) == Qt::Checked)
//		{
//			nsstring mapName = item->text(0).toStdString();
//			nsscene * newmap = nse.active()->get<nsscene>(mapName);
//			nsscene * curMap = nse.current_scene();

//			if (newmap == curMap)
//			{
//				accept();
//				return;
//			}

//			bool saveChanges = false;
//			if (curMap != NULL)
//			{
//                if (nse.active()->resource_changed(curMap))
//				{
//					QMessageBox mb(QMessageBox::Warning, "Save Current Map", "There are unsaved changes in the current map. Would you like to save before switching maps?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this);
//					int ret = mb.exec();
//					if (ret == QMessageBox::Cancel)
//						return;
//					saveChanges = (ret == QMessageBox::Yes);
//				}
//			}
//            nse.set_current_scene(newmap, false, saveChanges);
//			accept();
//			return;
//		}
//	}

//	nsscene * curMap = nse.current_scene();
//	bool saveChanges = false;
//    if (curMap != NULL && nse.active()->resource_changed(curMap))
//	{
//		QMessageBox mb(QMessageBox::Warning, "Save Current Map", "There are unsaved changes in the current map. Would you like to save before switching maps?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this);
//		int ret = mb.exec();
//		if (ret == QMessageBox::Cancel)
//			return;
//		saveChanges = (ret == QMessageBox::Yes);
//	}
//    nse.set_current_scene(nullptr, false, saveChanges);
//	nsengine * eng = &nse;
}

void ManageMapsDialog::onCancel()
{
	reject();
}
