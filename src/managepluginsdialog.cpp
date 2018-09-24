#include <qmessagebox.h>
#include <managepluginsdialog.h>
#include <qtreewidget.h>
#include <qstring.h>
#include <ui_plugindetailsdialog.h>
#include <qmessagebox.h>
#include <toolkit.h>
#include <qdatetime.h>
#include <qcolordialog.h>
#include <ui_editplugindialog.h>
#include <qrgb.h>
#include <sstream>

ManagePluginsDialog::ManagePluginsDialog(QWidget * parent):
QDialog(parent),
mNewPlugDialog(new QDialog(this)),
prevC(Qt::white),
mPressedState(Qt::Unchecked)
{

}

ManagePluginsDialog::~ManagePluginsDialog()
{
}

void ManagePluginsDialog::init()
{
	mUI.setupUi(this);
	addPluginsToTreeWidget();
	
	mNewPlugUI.setupUi(mNewPlugDialog);
	_setColorStyle();
	connect(mNewPlugUI.mDoneBtn, SIGNAL(pressed()), this, SLOT(onNewPluginAccept()));
	connect(mNewPlugUI.mCancelBtn, SIGNAL(pressed()), this, SLOT(onNewPluginReject()));
	connect(mUI.mPluginsTblW, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(onItemChanged(QTreeWidgetItem*, int)));
	connect(mNewPlugUI.mColorBtn, SIGNAL(clicked()), this, SLOT(onColorChange()));
}

/*!
Should call this when the dialog is opened - it will populate the plugins tree widget with all the current plugins
that are currently loaded in the engine
*/
void ManagePluginsDialog::addPluginsToTreeWidget()
{
//	mUI.mPluginsTblW->clear();

//	// Set up the active plugin stuff - if no active plugin is found then set to "None"
//	nsplugin * activePlug = nse.active();
//	if (activePlug != NULL)
//		mUI.mActivePluginLbl->setText(activePlug->name().c_str());
//	else
//		mUI.mActivePluginLbl->setText("None");

//	// The initail active plugin - if changed to something else this is used to detect the change
//	// so we can ask the user if they want to save changes to the map they were working on
//	mInitialActive = mUI.mActivePluginLbl->text();

//	// Populate the table widget with all the plugins found in the plugin manager
//	// The toolkit populates the plugins here on startup by using the plugins folder
//	auto iter = nse.plugins()->begin();
//	while (iter != nse.plugins()->end())
//	{
//		nsplugin * curPlug = nse.plugin(iter->first);
//		QTreeWidgetItem * item = new QTreeWidgetItem();
//		QString name((curPlug->name()).c_str());
//		item->setText(0, name);
//        item->setText(1, std::to_string(curPlug->resource_count()).c_str());
//		item->setCheckState(0, Qt::CheckState(int(curPlug->bound()) * 2));
//		mUI.mPluginsTblW->addTopLevelItem(item);
//		++iter;
//	}
}

/*!
Recursively check the checkbox of all parents of a plugin if the parents are loaded
If a parent is not loaded it will not be found and the function will return without
checking the box
*/
void ManagePluginsDialog::check(QTreeWidgetItem * pItem)
{
//	if (pItem == NULL)
//		return;
//	nsplugin * plug = nse.plugin(pItem->text(0).toStdString());
//	if (plug == NULL)
//		return;

//    nsstring_set parents = plug->parents();
//	auto parIter = parents.begin();
//	while (parIter != parents.end())
//	{
//		auto foundItems = mUI.mPluginsTblW->findItems(parIter->c_str(), Qt::MatchExactly);
//		if (foundItems.size() != 1)
//			return;
//		auto curItem = foundItems.first();
//		check(curItem);
//		++parIter;
//	}

//	// If make it all the way through it means parents are loaded - go ahead and check the box
//	pItem->setCheckState(0, Qt::Checked);
}

/*!
Recursive function which will uncheck all children of the plugin before unchecking the plugin
*/
void ManagePluginsDialog::uncheck(QTreeWidgetItem * pItem)
{
//	if (pItem == NULL)
//		return;
//	nsstring plugName = pItem->text(0).toStdString();

//	uint32 itemCount = mUI.mPluginsTblW->topLevelItemCount();
//	for (uint32 ind = 0; ind < itemCount; ++ind)
//	{
//		auto curItem = mUI.mPluginsTblW->topLevelItem(ind);
//		if (curItem == NULL)
//			return;

//		nsplugin * curPlug = nse.plugin(curItem->text(0).toStdString());
//		if (curPlug == NULL)
//			continue;

//        if (curPlug->has_parent(plugName))
//			uncheck(curItem);
//	}
//	pItem->setCheckState(0, Qt::Unchecked);
}

void ManagePluginsDialog::onColorChange()
{

	QColor c = QColorDialog::getColor(prevC, this);
	if (c.isValid())
		prevC = c;
	_setColorStyle();
}

void ManagePluginsDialog::onOkay()
{
//	if (mUI.mPluginsTblW->topLevelItemCount() < 0)
//		return;

//	nsstring activePluginName = mUI.mActivePluginLbl->text().toStdString();

//	if (activePluginName == "None")
//	{
//		QMessageBox mb(QMessageBox::Warning, "Active Plugin", "You need to have an active plugin set - all added resources will be saved to this plugin. Also, you can only load maps from the active plugin so please set one of the plugins to active, or create a new plugin.", QMessageBox::NoButton, this);
//		mb.exec();
//		return;
//	}

//	nsplugin_manager * plugs = nse.plugins();

//	if (!plugs->contains(activePluginName))
//	{
//		QMessageBox mb(QMessageBox::Warning, "Active Plugin", "There was an error finding the plugin that was set as active.. Could not find it in the set of available plugins. This is a bug and should be reported if you get this dialog.", QMessageBox::NoButton, this);
//		mb.exec();
//		return;
//	}

//	// If the active plugin has changed and the scene has unsaved changes ask the user if they want to save these changes
//	if (mInitialActive != mUI.mActivePluginLbl->text())
//	{
//		nsscene * curScene = nse.current_scene();
//        if (plugs->active() != NULL && nse.active()->resource_changed(curScene))
//		{
//			int ans;
//			QMessageBox mb(QMessageBox::Warning, "Plugin Changes", "There are unsaved changes to the active plugin - would you like to save or discard these changes?", QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, this);
//			ans = mb.exec();

//			if (ans == QMessageBox::Save)
//                bbtk.on_actionSave_triggered();
//			else if (ans == QMessageBox::Cancel)
//				return;
//			// Otherwise continue on!
//		}
//	}
//	nse.system<nsselection_system>()->clear();


//	// Build the list of plugins to load - check each one to make sure that it can be found before attempting to load
//	// If any cannot be found, dont load and add to the unloaded list. If any are already loaded, dont load and don't add
//	// to the unloaded list or loaded list. If unloaded and found, dont add to either list either right now. Only after
//	// actually loading should we add to loaded list
//	QVector<nsplugin*> plugsToLoad;
//	std::set<nsstring> unloadedPlugs;
//	std::set<nsstring> loadedPlugs;

//	uint32 itemCount = mUI.mPluginsTblW->topLevelItemCount();
//	for (uint32 index = 0; index < itemCount; ++index)
//	{
//		QTreeWidgetItem * qtwi = mUI.mPluginsTblW->topLevelItem(index);
//		nsstring plugName = qtwi->text(0).toStdString();
		
//		nsplugin * curPlug = plugs->get(plugName);
//		// Make sure plugin is found - wont be found for example if the user has deleted the plugin file
//		if (curPlug == NULL)
//		{
//			unloadedPlugs.emplace(plugName); // Dont care if succeeds or not - if the plugin is added twice it just means it is probably parent to another plugin
//			continue;
//		}

//		// If already has plugin, skip it
//		if (plugsToLoad.contains(curPlug))
//			continue;

//		AddParents(curPlug, plugsToLoad, unloadedPlugs, plugs);
//	}


//	// Set the current scene to NULL before unloading plugins
//    nse.set_current_scene(nullptr, false, false);

//	// Now go through and add the plugins and load the unloaded that are checked, unload the loaded that are unchecked
//	// But first, check to make sure every plugin has all of its necessary parent plugins before on the list
//	// We have to unload all
//	auto plugIter = plugsToLoad.begin();
//	while (plugIter != plugsToLoad.end())
//	{
//		nsstring plugName = (*plugIter)->name();
//		auto items = mUI.mPluginsTblW->findItems(plugName.c_str(), Qt::MatchExactly);
//		if (items.size() != 1)
//            throw std::exception();
//		auto curItem = items.first();

//		if (curItem->checkState(0) == Qt::Checked)
//			plugs->bind(plugName);
//		else
//			plugs->unbind(plugName);

//		++plugIter;
//	}

//	// Set the active plugin - if not found then throw exception
//    plugs->set_active(activePluginName);
//	nsplugin * activePlug = plugs->active();
//	if (activePlug == NULL)
//        throw std::exception();

//	bool openDialog = false;

//	// Now load a map from the active plugin.. if there is more than one map included bring up a dialog
//	// to pick which map to load, otherwise load the only map
//	// If there are no maps, then the current scene should remain NULL
//	nsscene_manager * sm = activePlug->manager<nsscene_manager>();
//	if (!sm->empty())
//	{
//		if (sm->count() > 1)
//			openDialog = true;
//		else
//		{
//            nse.set_current_scene(sm->begin()->first, false, false);
//			nsscene * cur = nse.current_scene();
//            if (!cur->has_dir_light())
//			{
//                nsentity * dl = activePlug->create_dir_light(cur->name() + "dlight", 0.6f, 0.3f);
//				cur->add(dl, fvec3(20.0f, 20.0f, -40.0f), ::orientation(fvec4(0, 1, 0, 30)));
//			}
//			if (cur->camera() == NULL)
//			{
//                nsentity * cam = activePlug->create_camera(cur->name() + "cam", 60.0f, uivec2(bbtk.map()->width(), bbtk.map()->height()), fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
//                cur->set_camera(cam, true);
//			}
//		}
//	}

//	// Refresh toolkit views and close dialog
//    bbtk.refresh_views();
//	accept();
//	if (openDialog)
//        bbtk.on_actionSwitchMap_triggered();
}

void ManagePluginsDialog::onCancel()
{
	reject();
}

void ManagePluginsDialog::onNewPlugin()
{
	if (mNewPlugDialog->exec() == QDialog::Accepted)
		addPluginsToTreeWidget();	
}


void ManagePluginsDialog::onNewPluginAccept()
{
//	nsstring name = mNewPlugUI.mResNameLE->text().toStdString();
//	nsstring creator = mNewPlugUI.mCreatorNameLE->text().toStdString();
//	nsstring plugNotes = mNewPlugUI.mDescriptionTextEdit->toPlainText().toStdString();
//	nsplugin_manager * plugs = nse.plugins();

//	if (name.empty())
//	{
//		QMessageBox mb(QMessageBox::Warning, "Plugin Name Error", "Enter a name for the plugin", QMessageBox::NoButton, this);
//		mb.exec();
//		return;
//	}

//	if (plugs->contains(name))
//	{
//		QMessageBox mb(QMessageBox::Warning, "Plugin Name Error", "There is already a plugin with this name. Please use a different name.", QMessageBox::NoButton, this);
//		mb.exec();
//		return;
//	}

//	// Do the scene name check before making the plugin so that we dont have to erase the plugin
//	// if the user forgot to enter the scene name
//	if (mNewPlugUI.mSceneGB->isChecked() && mNewPlugUI.mSceneNameLE->text().isEmpty())
//	{
//		QMessageBox mb(QMessageBox::Warning, "Map Name Error", "Enter a name for the map", QMessageBox::NoButton, this);
//		mb.exec();
//		return;
//	}

//	// From here on out we have to remove the plugin if something goes wrong
//	nsplugin * plug = plugs->create(name);
//	if (plug == NULL)
//	{
//		QMessageBox mb(QMessageBox::Warning, "Plugin Error", "There was an error in creating plugin with name " + QString(name.c_str()) + ". Try a different name", QMessageBox::NoButton, this);
//		mb.exec();
//		return;
//	}

//	QDate date = QDate::currentDate();
//    plug->set_creator(creator);
//    plug->set_notes(plugNotes);
//    plug->set_edit_date(date.toString().toStdString());
//    plug->set_creation_date(date.toString().toStdString());
//    nse.set_active(plug);
//	mUI.mActivePluginLbl->setText(name.c_str());

//	// If the user wants a default map, we will make one here..
//	if (mNewPlugUI.mSceneGB->isChecked())
//	{
//		nsstring sceneName = mNewPlugUI.mSceneNameLE->text().toStdString();
//		nsstring sceneCreator = mNewPlugUI.mSceneCreatorLE->text().toStdString();
//		nsstring sceneNotes = mNewPlugUI.mSceneNotesPTE->toPlainText().toStdString();
//		uint32 max_players = mNewPlugUI.mMaxPlayersSB->value();
//		QColor prev = mNewPlugUI.mColorBtn->palette().color(backgroundRole());
//		fvec3 color = fvec3(prev.redF(), prev.greenF(), prev.blueF());

//		if (plug->contains<nsscene>(sceneName))
//		{
//			QMessageBox mb(QMessageBox::Warning, "Map Name Error", "There is already a map \"" + QString(sceneName.c_str()) + "\". Please use a different map name.", QMessageBox::NoButton, this);
//			mb.exec();
//            nse.destroy_plugin(plug);
//            nse.set_active(nullptr);
//			return;
//		}

//		nsscene * scene = plug->create<nsscene>(sceneName);
//		if (scene == NULL)
//		{
//			QMessageBox mb(QMessageBox::Warning, "Map Error", "There was an error in creating map \"" + QString(sceneName.c_str()) + "\". You will have to make a new map.", QMessageBox::NoButton, this);
//			mb.exec();
//            nse.destroy_plugin(plug);
//            nse.set_active(nullptr);
//		}

//		// Set up the scene with the values as determined from the dialog
//        scene->set_max_players(max_players);
//        scene->set_creator(sceneCreator);
//        scene->set_notes(sceneNotes);
//        scene->set_bg_color(color);
//		plug->save(scene);
//	}

//	// Clear the vals for next time use (we dont delete the dialog)
//	mNewPlugUI.mCreatorNameLE->clear();
//	mNewPlugUI.mDescriptionTextEdit->clear();
//	mNewPlugUI.mResNameLE->clear();
//	mNewPlugUI.mSceneCreatorLE->clear();
//	mNewPlugUI.mSceneNotesPTE->clear();
//	mNewPlugUI.mSceneNameLE->clear();
//	mNewPlugDialog->accept();
}

void ManagePluginsDialog::onNewPluginReject()
{
	mNewPlugUI.mCreatorNameLE->clear();
	mNewPlugUI.mDescriptionTextEdit->clear();
	mNewPlugUI.mResNameLE->clear();
	mNewPlugDialog->reject();
}

/*!
This changes the right portion of the UI based on which plugin is selected - ie the notes and description fields
*/
void ManagePluginsDialog::onSelectionChange()
{
//	mUI.mParentsTW->clear();
//	auto items = mUI.mPluginsTblW->selectedItems();
//	if (items.empty())
//		return;

//	QTreeWidgetItem * item = items.first();

//	if (item == NULL)
//		return;

//	nsplugin * plug = nse.plugin(item->text(0).toStdString());

//	if (plug == NULL)
//		return;

//    mUI.mNotesTextEdit->setPlainText(plug->notes().c_str());
//	mUI.mCreatorLE->setText(plug->creator().c_str());
//    mUI.mDateCreatedLbl->setText(plug->creation_date().c_str());
//    mUI.mDateModifiedLbl->setText(plug->edit_date().c_str());

//    const nsstring_set & rents = plug->parents();
//	auto pIter = rents.begin();
//	while (pIter != rents.end())
//	{
//		QTreeWidgetItem * tItem = new QTreeWidgetItem();
//		tItem->setText(0, (*pIter).c_str());
//		mUI.mParentsTW->addTopLevelItem(tItem);
//		++pIter;
//	}
}

/*!
Every time an item is changed in the tree widget this slot is called - in particular
an item will "change" when its check state is changed
*/
void ManagePluginsDialog::onItemChanged(QTreeWidgetItem* item, int pColumn)
{
//	nsstring plugName = item->text(0).toStdString();

//	nsplugin * curPlug = nse.plugin(plugName);
//	if (curPlug == NULL)
//        throw std::exception();

//	if (pColumn == 0 && item->checkState(pColumn) == Qt::Unchecked)
//	{
//		// Go through and check each plugin to see if the unchecked plugin is a parent to any of them.. if it is then it cannot be unchecked
//		uint32 itemCount = mUI.mPluginsTblW->topLevelItemCount();
//		for (uint32 index = 0; index < itemCount; ++index)
//		{
//			QTreeWidgetItem * qtwi = mUI.mPluginsTblW->topLevelItem(index);
//			nsstring plugName = qtwi->text(pColumn).toStdString();

//			nsplugin * curPlug = nse.plugin(plugName);
//			if (curPlug == NULL)
//				continue;

//			// If current item is checked, check the parent plugins make sure they are enabled
//			if (qtwi->checkState(pColumn) == Qt::Checked)
//			{
//				if (_itemHasCheckedChildren(item))
//				{
//					item->setCheckState(0, Qt::Checked);
//					QMessageBox mb(QMessageBox::Warning, "Parent Plugin", "You cannot disable " + item->text(0) + " because it is a parent plugin to another enabled plugin. Disable the other plugins with this plugin as thier parent first.", QMessageBox::NoButton, this);
//					mb.exec();
//					return;
//				}
//			}
//		}

//		// After the previous two checks we now must set the active plugin to "None" if the unchecked plugin was set to active
//		if (item->text(pColumn) == mUI.mActivePluginLbl->text())
//		{
//			mUI.mActivePluginLbl->setText("None");
//			return;
//		}

//	}
//	else if (pColumn == 0 && item->checkState(pColumn) == Qt::Checked)
//	{

//		// Make sure that all parents are activated when a child plugin is enabled
//        nsstring_set parents = curPlug->parents();
//		auto iter = parents.begin();
//		while (iter != parents.end())
//		{
//			auto matchItems = mUI.mPluginsTblW->findItems((*iter).c_str(), Qt::MatchExactly);

//			// Better not be more than one match
//			if (matchItems.size() > 1)
//                throw std::exception();

//			if (matchItems.isEmpty())
//			{
//				// If the plugin being enabled has a parent plugin that cannot be found in the plugins
//				// tbl widget and the parents are not loaded it means that we should definitely not enable this plugin
//                if (!curPlug->parents_loaded())
//				{
//					item->setCheckState(0, Qt::Unchecked);
//					QMessageBox mb(QMessageBox::Warning, "Parent Plugin Error", "You cannot enable " + item->text(0) + " because its parent plugins cannot be found - add parent plugins to plugins folder.", QMessageBox::NoButton, this);
//					mb.exec();
//				}
//				return;
//			}

//			// Set the parent plugin to activated
//			auto parentItem = matchItems.first();
//			parentItem->setCheckState(0, Qt::Checked);

//			// If the parent plugin is set as the active plugin then switch the active plugin
//			// to this plugin that is being enabled
//			if (mUI.mActivePluginLbl->text() == parentItem->text(0))
//				mUI.mActivePluginLbl->setText(item->text(0));

//			++iter;
//		}
//	}
}

/*!
Set the highlighted plugin to active - just changes the text in the active plugin section
Doesnt actually set the plugin to active until the okay button is pressed
*/
void ManagePluginsDialog::onSetActive()
{
//	auto items = mUI.mPluginsTblW->selectedItems();
//	if (items.empty())
//		return;

//	QTreeWidgetItem * item = items.first();

//	if (item == NULL)
//		return;

//	// Have to make sure that the plugin that is set to active has all plugins that depend on it unloaded..
//	// Go through all plugs in tbl widget and check if they have a parent with the name of the plugin
//	// that is trying to be set as active.. if they do they uncheck them..
//	nsstring plugName = item->text(0).toStdString();
//	uint32 itemCount = mUI.mPluginsTblW->topLevelItemCount();
//	for (uint32 i = 0; i < itemCount; ++i)
//	{
//		// All operations on column 0
//		auto tmpItem = mUI.mPluginsTblW->topLevelItem(i);
//		if (tmpItem == NULL)
//			continue;
//		nsstring tmpPlugName = tmpItem->text(0).toStdString();
//		nsplugin * plug = nse.plugin(tmpPlugName);
//		if (plug == NULL)
//			continue;
//        if (plug->has_parent(plugName))
//			uncheck(tmpItem);
//	}


//	// Set the check state to checked to indicate that it will be loaded
//	item->setCheckState(0, Qt::Checked);

//	// Set the label to say the plugins name
//	mUI.mActivePluginLbl->setText(item->text(0));
}

void ManagePluginsDialog::onDeletePlugin()
{
//	auto items = mUI.mPluginsTblW->selectedItems();
//	if (items.empty())
//		return;

//	QTreeWidgetItem * item = items.first();

//	if (item == NULL)
//		return;

//	nsplugin * plug = nse.plugin(item->text(0).toStdString());

//	if (plug == NULL)
//		return;

//	if (_itemHasCheckedChildren(item))
//	{
//		QMessageBox mb(QMessageBox::Warning, "Parent Plugin", "You cannot delete " + item->text(0) + " because it is a parent plugin to another enabled plugin. Disable the other plugins with this plugin as thier parent first.", QMessageBox::NoButton, this);
//		mb.exec();
//		return;
//	}

//	QMessageBox mb(this);
//	mb.setText("Do you want to delete all resources associated with this plugin also? This will permanently remove them from the hard drive.");
//	mb.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
//	int ret = mb.exec();
//	if (ret == QMessageBox::Yes)
//	{
//        nse.del_plugin(plug);
//		addPluginsToTreeWidget();
//	}
//	else if (ret == QMessageBox::No)
//	{
//        nse.destroy_plugin(plug);
//		addPluginsToTreeWidget();
//	}
//	else
//	{
//		return;
//	}
}

/*!
Brings up a details dialog which shows all the resources included in the plugin - does not include resources in parent
plugins
*/
void ManagePluginsDialog::onDetails()
{
//	QDialog * det = new QDialog(this);
//	Ui_PluginDetailsDialog pDet;
//	pDet.setupUi(det);

//	auto items = mUI.mPluginsTblW->selectedItems();
//	if (items.empty())
//		return;

//	QTreeWidgetItem * item = items.first();

//	if (item == NULL)
//		return;

//	nsplugin * plug = nse.plugin(item->text(0).toStdString());

//	if (plug == NULL)
//		return;

//    pDet.mText->setPlainText(plug->details().c_str());
//	det->exec();
}

void ManagePluginsDialog::onEditPlugin()
{
//	QDialog * editPlug = new QDialog(this);
//	Ui_EditPluginDialog editUI;
//	editUI.setupUi(editPlug);

//	auto items = mUI.mPluginsTblW->selectedItems();
//	if (items.empty())
//		return;

//	QTreeWidgetItem * item = items.first();

//	if (item == NULL)
//		return;

//	nsplugin * plug = nse.plugin(item->text(0).toStdString());

//	if (plug == NULL)
//		return;

//	editUI.mCreatorNameLE->setText(plug->creator().c_str());
//    editUI.mDescriptionTextEdit->setPlainText(plug->notes().c_str());
//	editUI.mResNameLE->setText(plug->name().c_str());

//	if (editPlug->exec() == QDialog::Accepted)
//	{
//		nsstring newPlugName = editUI.mResNameLE->text().toStdString();
//		if (plug->name() != newPlugName)
//		{
//			QMessageBox mb(this);
//			mb.setText("Are you sure you want to rename the plugin? The toolkit will automatically propagate this name change to plugins that depend on this one that are located in the plugins folder. However, if there"
//				" are plugins that are outside of the plugins folder that depend on this one, they will be unable to find this plugin. So, are you sure?");
//			mb.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
//			if (mb.exec() == QMessageBox::Yes)
//				nse.plugins()->rename(plug->name(), newPlugName);
//		}

//		nsstring newNotes = editUI.mDescriptionTextEdit->toPlainText().toStdString();
//        plug->set_notes(newNotes);

//		nsstring newCreator = editUI.mCreatorNameLE->text().toStdString();
//        plug->set_creator(newCreator);
//		addPluginsToTreeWidget();
//	}
}

bool ManagePluginsDialog::_itemHasCheckedChildren(QTreeWidgetItem* item)
{
//	nsplugin * curPlug = nse.plugin(item->text(0).toStdString());

//	if (curPlug == NULL)
//		return false;

//    nsstring_set parents = curPlug->parents();
//	auto iter = parents.begin();
//	nsstring itemStr = item->text(0).toStdString();
//	while (iter != parents.end())
//	{
//		if ((*iter) == itemStr)
//			return true;
//		++iter;
//	}

//	return false;
}

/*!
Add each parent of the plugin to the list, recursively calling this function on each parent.
*/
//void AddParents(nsplugin * pCurPlug, QVector<nsplugin*> & pPlugsToLoad, std::set<nsstring> & pUnloadedPlugs, nsplugin_manager * pPlugs)
//{
//	if (pCurPlug->parents().size() > 0)
//	{
//        nsstring_set ps = pCurPlug->parents();
//		auto iter = ps.begin();
//		while (iter != ps.end())
//		{
//			nsplugin * parentPlug = nse.plugin(*iter);
//			// Make sure plugin is found - wont be found for example if the user has deleted the plugin file
//			if (parentPlug == NULL)
//			{
//				pUnloadedPlugs.emplace(*iter); // Dont care if succeeds or not - if the plugin is added twice it just means it is probably parent to another plugin
//				++iter;
//				continue;
//			}

//			// Add the parents of the parent plugin if the parent plugin has not already been added
//			if (!pPlugsToLoad.contains(parentPlug))
//				AddParents(parentPlug, pPlugsToLoad, pUnloadedPlugs, pPlugs);;

//			++iter;
//		}
//	}

//	// Now add this plugin to the to load vector if its not there and not bound
//	if (!pPlugsToLoad.contains(pCurPlug) && !nse.plugin(pCurPlug)->bound())
//		pPlugsToLoad.push_back(pCurPlug);
//}

void ManagePluginsDialog::_setColorStyle()
{
	QString rgbcol = "rgb(" + QString::number(prevC.red()) + "," + QString::number(prevC.green()) + "," + QString::number(prevC.blue()) + ");";
	mNewPlugUI.mColorBtn->setStyleSheet(
		"QFrame {\
		background-color: " + rgbcol +
		"border: none\
		}\
		QFrame:hover\
		{\
		background-color: rgb(200,200,200);\
		}");
}
