#include <newmapdialog.h>
#include <QMessageBox>
#include <QColorDialog>
#include <toolkit.h>
//#include <ui_selecttiledialog.h>
//#include <ui_select_res_dialog.h>
#include <listwidgetcust.h>

NewMapDialog::NewMapDialog(QWidget * parent) :
prevC(Qt::white),
QDialog(parent)
{}

NewMapDialog::~NewMapDialog()
{}

void NewMapDialog::init()
{
	mNewMapUI.setupUi(this);
	_setColorStyle();
	connect(mNewMapUI.mColorBtn, SIGNAL(clicked()), this, SLOT(onColorChange()));
}

void NewMapDialog::onChooseSkybox()
{
//	QDialog srd(this);

//    Ui::select_res_dialog ui;
//	ui.setupUi(&srd);

//	// Go through and add entities to the list widget which have camera component
//	auto plugiter = nse.plugins()->begin();
//	while (plugiter != nse.plugins()->end())
//	{
//		nsplugin * plg = nse.plugin(plugiter->first);
//		auto entiter = plg->manager<nsentity_manager>()->begin();
//		while (entiter != plg->manager<nsentity_manager>()->end())
//		{
//			nsentity * curent = plg->get<nsentity>(entiter->first);
//			nsrender_comp * rc = curent->get<nsrender_comp>();
//			if (rc != NULL)
//			{
//                nsmaterial * mat = nse.resource<nsmaterial>(rc->material_id(0));
//				if (mat != NULL)
//				{
//                    nstexture * cm = nse.resource<nstexture>(mat->map_tex_id(nsmaterial::diffuse));
//                    if (cm != NULL && type_to_hash(*cm) == type_to_hash(nstex_cubemap))
//					{
//						QListWidgetItem * item = new QListWidgetItem(curent->name().c_str());
//                        item->setData(VIEW_WIDGET_ITEM_PLUG, curent->plugin_id());
//						item->setData(VIEW_WIDGET_ITEM_ENT, curent->id());
//                        ui.m_lw->addItem(item);
//					}
//				}
//			}
//			++entiter;
//		}
//		++plugiter;
//	}

//	// Now open up the basic list widget dialog to allow the user to select which
//	// camera to use
//	if (srd.exec() == QDialog::Accepted)
//	{
//        auto selitems = ui.m_lw->selectedItems();
//		if (selitems.isEmpty())
//			return;
//		auto fitem = selitems.first();
//		uivec2 skyboxid(fitem->data(VIEW_WIDGET_ITEM_PLUG).toUInt(), fitem->data(VIEW_WIDGET_ITEM_ENT).toUInt());

//		nsentity * skyboxent = nse.resource<nsentity>(skyboxid);
//		mNewMapUI.mSkyboxLE->setText(skyboxent->name().c_str());
//        mNewMapUI.mSkyboxPluginLE->setText(nse.plugin(skyboxent->plugin_id())->name().c_str());
//	}
}

void NewMapDialog::onChooseTile()
{
//	QDialog srd(this);

//    Ui::select_res_dialog ui;
//	ui.setupUi(&srd);

//	// Go through and add entities to the list widget which have tile component
//	auto plugiter = nse.plugins()->begin();
//	while (plugiter != nse.plugins()->end())
//	{
//		nsplugin * plg = nse.plugin(plugiter->first);
//		auto entiter = plg->manager<nsentity_manager>()->begin();
//		while (entiter != plg->manager<nsentity_manager>()->end())
//		{
//			nsentity * curent = plg->get<nsentity>(entiter->first);
//			if (curent->has<nstile_comp>())
//			{
//				QListWidgetItem * item = new QListWidgetItem(curent->name().c_str());
//                item->setData(VIEW_WIDGET_ITEM_PLUG, curent->plugin_id());
//				item->setData(VIEW_WIDGET_ITEM_ENT, curent->id());
//                ui.m_lw->addItem(item);
//			}
//			++entiter;
//		}
//		++plugiter;
//	}

//	// Now open up the basic list widget dialog to allow the user to select which
//	// directional light to use
//	if (srd.exec() == QDialog::Accepted)
//	{
//        auto selitems = ui.m_lw->selectedItems();
//		if (selitems.isEmpty())
//			return;
//		auto fitem = selitems.first();
//		uivec2 tileid(fitem->data(VIEW_WIDGET_ITEM_PLUG).toUInt(), fitem->data(VIEW_WIDGET_ITEM_ENT).toUInt());

//		nsentity * tileent = nse.resource<nsentity>(tileid);
//		mNewMapUI.mTileNameLE->setText(tileent->name().c_str());
//        mNewMapUI.mTilePluginLE->setText(nse.plugin(tileent->plugin_id())->name().c_str());
//	}
}

void NewMapDialog::onNewSkybox()
{
	// TODO: Once new skybox function is written reference it here
}

void NewMapDialog::onCameraCB(bool var)
{
	mNewMapUI.mCamLE->setEnabled(!var);
	mNewMapUI.mCamPluginLE->setEnabled(!var);
	mNewMapUI.mNewCamBtn->setEnabled(!var);
	mNewMapUI.mChooseCamBtn->setEnabled(!var);
	mNewMapUI.mCamNameLbl->setEnabled(!var);
	mNewMapUI.mCamPluginLbl->setEnabled(!var);
}

void NewMapDialog::onDirLightCB(bool var)
{
	mNewMapUI.mDirLightLE->setEnabled(!var);
	mNewMapUI.mLightPluginLE->setEnabled(!var);
	mNewMapUI.mNewDirLightBtn->setEnabled(!var);
	mNewMapUI.mChooseLightBtn->setEnabled(!var);
	mNewMapUI.mLightNameLbl->setEnabled(!var);
	mNewMapUI.mLightPluginLbl->setEnabled(!var);
}

void NewMapDialog::onChooseCamera()
{
//	QDialog srd(this);

//    Ui::select_res_dialog ui;
//	ui.setupUi(&srd);

//	// Go through and add entities to the list widget which have camera component
//	auto plugiter = nse.plugins()->begin();
//	while (plugiter != nse.plugins()->end())
//	{
//		nsplugin * plg = nse.plugin(plugiter->first);
//		auto entiter = plg->manager<nsentity_manager>()->begin();
//		while (entiter != plg->manager<nsentity_manager>()->end())
//		{
//			nsentity * curent = plg->get<nsentity>(entiter->first);
//            if (curent->has<nscam_comp>())
//			{
//				QListWidgetItem * item = new QListWidgetItem(curent->name().c_str());
//                item->setData(VIEW_WIDGET_ITEM_PLUG, curent->plugin_id());
//				item->setData(VIEW_WIDGET_ITEM_ENT, curent->id());
//                ui.m_lw->addItem(item);
//			}
//			++entiter;
//		}
//		++plugiter;
//	}

//	// Now open up the basic list widget dialog to allow the user to select which
//	// camera to use
//	if (srd.exec() == QDialog::Accepted)
//	{
//        auto selitems = ui.m_lw->selectedItems();
//		if (selitems.isEmpty())
//			return;
//		auto fitem = selitems.first();
//		uivec2 camid(fitem->data(VIEW_WIDGET_ITEM_PLUG).toUInt(), fitem->data(VIEW_WIDGET_ITEM_ENT).toUInt());

//		nsentity * cament = nse.resource<nsentity>(camid);
//		mNewMapUI.mCamLE->setText(cament->name().c_str());
//        mNewMapUI.mCamPluginLE->setText(nse.plugin(cament->plugin_id())->name().c_str());
//	}
}

void NewMapDialog::onNewCamera()
{
	//TODO: Once new camera entity function is written write reference to it here
}

void NewMapDialog::onChooseDirlight()
{
//	QDialog srd(this);

//    Ui::select_res_dialog ui;
//	ui.setupUi(&srd);

//	// Go through and add entities to the list widget which have light component
//	// and light component is of type "DirLight"
//	auto plugiter = nse.plugins()->begin();
//	while (plugiter != nse.plugins()->end())
//	{
//		nsplugin * plg = nse.plugin(plugiter->first);
//		auto entiter = plg->manager<nsentity_manager>()->begin();
//		while (entiter != plg->manager<nsentity_manager>()->end())
//		{
//			nsentity * curent = plg->get<nsentity>(entiter->first);
//			nslight_comp * lc = curent->get<nslight_comp>();
//            if (lc != NULL && lc->type() == nslight_comp::l_dir)
//			{
//				QListWidgetItem * item = new QListWidgetItem(curent->name().c_str());
//                item->setData(VIEW_WIDGET_ITEM_PLUG, curent->plugin_id());
//				item->setData(VIEW_WIDGET_ITEM_ENT, curent->id());
//                ui.m_lw->addItem(item);
//			}
//			++entiter;
//		}
//		++plugiter;
//	}

//	// Now open up the basic list widget dialog to allow the user to select which
//	// directional light to use
//	if (srd.exec() == QDialog::Accepted)
//	{
//        auto selitems = ui.m_lw->selectedItems();
//		if (selitems.isEmpty())
//			return;
//		auto fitem = selitems.first();
//		uivec2 lid(fitem->data(VIEW_WIDGET_ITEM_PLUG).toUInt(), fitem->data(VIEW_WIDGET_ITEM_ENT).toUInt());

//		nsentity * lightent = nse.resource<nsentity>(lid);
//		mNewMapUI.mDirLightLE->setText(lightent->name().c_str());
//        mNewMapUI.mLightPluginLE->setText(nse.plugin(lightent->plugin_id())->name().c_str());
//	}
}

void NewMapDialog::onNewDirlight()
{
	// TODO: Once new directional light entity type function is written - (including dialog) - then reference it here
}

void NewMapDialog::onNewTile()
{
	// NewTileDialog newTileDialog(this);
    // newTileDialog.init();
	// if (newTileDialog.exec() == QDialog::Accepted)
	// {
    //     bbtk.refresh_views();
	// 	QString name = newTileDialog.getEntityName();
	// 	mNewMapUI.mTileNameLE->setText(name);
	// }
}

void NewMapDialog::onColorChange()
{
	QColor c = QColorDialog::getColor(prevC);
	if (c.isValid())
		prevC = c;
	_setColorStyle();
}

void NewMapDialog::onCreate()
{
//	nsstring mapName = mNewMapUI.mMapNameLE->text().toStdString();
//	nsstring mapCreator = mNewMapUI.mCreatorLE->text().toStdString();
//	nsstring mapNotes = mNewMapUI.mSceneNotesPTE->toPlainText().toStdString();
//	uint32 max_players = mNewMapUI.mMaxPlayersSB->value();
//	fvec3 color = fvec3(prevC.redF(), prevC.greenF(), prevC.blueF());

//	nsplugin * actPlug = nse.active();
//	if (actPlug == NULL)
//	{
//		QMessageBox mb(QMessageBox::Warning, "Plugin Error", "No active plugin detected - cannot create map.", QMessageBox::NoButton, this);
//		mb.exec();
//		return;
//	}

//	if (mapName.empty())
//	{
//		QMessageBox mb(QMessageBox::Warning, "Map Name Error", "Enter a name for the map", QMessageBox::NoButton, this);
//		mb.exec();
//		return;
//	}

//	nsscene_manager * scenes = actPlug->manager<nsscene_manager>();
//	if (scenes->contains(mapName))
//	{
//		QMessageBox mb(QMessageBox::Warning, "Map Name Error", "There is already a map with this name. Please use a different name.", QMessageBox::NoButton, this);
//		mb.exec();
//		return;
//	}

//	nsentity * tile = NULL;
//	if (mNewMapUI.mLoadGridGB->isChecked())
//	{
//		nsstring tName = mNewMapUI.mTileNameLE->text().toStdString();
//		nsstring plugName = mNewMapUI.mTilePluginLE->text().toStdString();
//		tile = nse.resource<nsentity>(plugName, tName);
//		if (tile == NULL)
//		{
//			QMessageBox mb(QMessageBox::Warning, "Tile Name Error", "Please specify a valid tile if you want to load a base grid.", QMessageBox::NoButton, this);
//			mb.exec();
//			return;
//		}
//	}

//	nsentity * skyb = NULL;
//	if (mNewMapUI.mLoadSkyboxGB->isChecked())
//	{
//		nsstring sname = mNewMapUI.mSkyboxLE->text().toStdString();
//		nsstring plugName = mNewMapUI.mSkyboxPluginLE->text().toStdString();
//		skyb = nse.resource<nsentity>(plugName, sname);
//		if (skyb == NULL)
//		{
//			QMessageBox mb(QMessageBox::Warning, "Skybox Name Error", "Please specify a valid skybox if you want to load a skybox with the map", QMessageBox::NoButton, this);
//			mb.exec();
//			return;
//		}
//	}

//	nsentity * dirLight = NULL;
//	if (!mNewMapUI.mCreateDefaultDirLightCB->isChecked())
//	{
//		nsstring name = mNewMapUI.mDirLightLE->text().toStdString();
//		nsstring plugName = mNewMapUI.mLightPluginLE->text().toStdString();
//		dirLight = nse.resource<nsentity>(plugName, name);
//		if (dirLight == NULL)
//		{
//			QMessageBox mb(QMessageBox::Warning, "Directional Light Error", "Please specify a valid directional light if you want to load a custom one", QMessageBox::NoButton, this);
//			mb.exec();
//			return;
//		}
//	}
//	else
//        dirLight = nse.active()->create_dir_light(mapName + "_dlight", 0.6f, 0.3f);

//	nsentity * scenecam = NULL;
//	if (!mNewMapUI.mCreateDefaultCamCB->isChecked())
//	{
//		nsstring name = mNewMapUI.mCamLE->text().toStdString();
//		nsstring plugName = mNewMapUI.mCamPluginLE->text().toStdString();
//		scenecam = nse.resource<nsentity>(plugName, name);
//		if (scenecam == NULL)
//		{
//			QMessageBox mb(QMessageBox::Warning, "Camera Error", "Please specify a valid camera if you want to load a custom one", QMessageBox::NoButton, this);
//			mb.exec();
//			return;
//		}
//	}
//	else
//        scenecam = nse.active()->create_camera(mapName + "_cam", 60.0f, uivec2(bbtk.map()->width(), bbtk.map()->height()), fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));

//	nsscene * scene = scenes->create(mapName);
//	if (scene == NULL)
//	{
//		QMessageBox mb(QMessageBox::Warning, "Map Error", "There was an error in creating map with name " + QString(mapName.c_str()) + ". Try a different name", QMessageBox::NoButton, this);
//		mb.exec();
//		return;
//	}

//    scene->set_creator(mapCreator);
//    scene->set_notes(mapNotes);
//    scene->set_max_players(max_players);
//    scene->set_bg_color(color);

//	nsscene * curScene = nse.current_scene();
//    nse.set_current_scene(scene, true, true);
//	if (tile != NULL)
//	{
//		ivec3 bounds(mNewMapUI.mGridXSB->value(), mNewMapUI.mGridYSB->value(), mNewMapUI.mHeightSB->value());
//        scene->add_gridded(tile, bounds);
//		actPlug->save(scene);
//	}
//	if (skyb != NULL)
//        scene->set_skydome(skyb, true);
//    scene->set_camera(scenecam, true);
//	scene->add(dirLight, fvec3(20.0f, 20.0f, -40.0f), ::orientation(fvec4(0, 1, 0, 10)));
//    nse.set_current_scene(curScene, false, true);
//	accept();
}

void NewMapDialog::onCancel()
{
	reject();
}

void NewMapDialog::_setColorStyle()
{
	QString rgbcol = "rgb(" + QString::number(prevC.red()) + "," + QString::number(prevC.green()) + "," + QString::number(prevC.blue()) + ");";
	mNewMapUI.mColorBtn->setStyleSheet(
		"QFrame {\
		background-color: " + rgbcol +
		"border: none\
		}\
		QFrame:hover\
		{\
		background-color: rgb(200,200,200);\
		}");
}
