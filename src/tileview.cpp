#include <prefab_editor_dialog.h>
#include <toolkit.h>
#include <tileview.h>

TileView::TileView(QWidget * parent):
QMainWindow(parent)
{
    m_ui.setupUi(this);	
}

TileView::~TileView()
{
}

void TileView::init()
{
}

void TileView::onActionNew()
{
    Prefab_Editor_Dialog * pfe = bbtk.prefab_editor();
    pfe->setModal(false);
    pfe->show();
}

void TileView::onActionDelete()
{

}

void TileView::onActionEdit()
{

}

void TileView::refresh()
{
//	reset();

//	auto plugiter = nse.plugins()->begin();
//	while (plugiter != nse.plugins()->end())
//	{
//		nsplugin * plg = nse.plugin(plugiter->first);
//		auto iter = plg->manager<nsentity_manager>()->begin();
//		while (iter != plg->manager<nsentity_manager>()->end())
//		{
//			nsentity * ent = (nsentity*)iter->second;
//			if (ent->has<nstile_comp>() && !ent->has<nstile_brush_comp>())
//			{
//				QListWidgetItem * item = new QListWidgetItem();

//				// Use resource name as the name shown in window
//				item->setText(ent->name().c_str());

//				// Store the resource id and plug id for easy lookup later
//                item->setData(VIEW_WIDGET_ITEM_PLUG, ent->plugin_id());
//				item->setData(VIEW_WIDGET_ITEM_ENT, ent->id());
				
//                if (!ent->icon_path().empty())
//                    item->setIcon(QIcon(ent->icon_path().c_str()));
//				else
//                    item->setIcon(QIcon(":/ResourceIcons/icons/default_hex.png"));

//                m_ui.mListWidget->addItem(item);
//			}
//			++iter;
//		}
//		++plugiter;
//	}
}

void TileView::onSelectionChanged()
{
//	QListWidgetItem * item = NULL;
//    auto items = m_ui.mListWidget->selectedItems();
//	if (!items.isEmpty())
//		item = items[0];

//	if (item == NULL)
//	{
//        nse.system<nsbuild_system>()->set_tile_build_ent(NULL);
//		return;
//	}

//	uivec2 itid(item->data(VIEW_WIDGET_ITEM_PLUG).toUInt(), item->data(VIEW_WIDGET_ITEM_ENT).toUInt());
//	nsentity * ent = nse.resource<nsentity>(itid);
//    nse.system<nsbuild_system>()->set_tile_build_ent(ent);
}

void TileView::reset()
{
    m_ui.mListWidget->clear();
}
