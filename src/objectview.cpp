#include <objectview.h>

ObjectView::ObjectView(QWidget * parent) :
QMainWindow(parent)
{
	m_ui.setupUi(this);
}

ObjectView::~ObjectView()
{

}

void ObjectView::init()
{
}

void ObjectView::refresh()
{
//    m_ui.mListWidget->clear();

//	auto plugiter = nse.plugins()->begin();
//	while (plugiter != nse.plugins()->end())
//	{
//		nsplugin * plg = nse.plugin(plugiter->first);

//		auto iter = plg->manager<nsentity_manager>()->begin();
//		while (iter != plg->manager<nsentity_manager>()->end())
//		{
//			nsentity * ent = (nsentity*)iter->second;
//			QListWidgetItem * item = new QListWidgetItem();
//			item->setText(ent->name().c_str());
//            if (!ent->icon_path().empty())
//                item->setIcon(QIcon(ent->icon_path().c_str()));
//            else
//                item->setIcon(QIcon(":/ResourceIcons/icons/default_object.png"));

//            item->setData(VIEW_WIDGET_ITEM_PLUG, ent->plugin_id());
//			item->setData(VIEW_WIDGET_ITEM_ENT, ent->id());

//            m_ui.mListWidget->addItem(item);
//			++iter;
//		}

//		++plugiter;
//	}
}

void ObjectView::onActionNew()
{
}

void ObjectView::onActionDelete()
{
}

void ObjectView::onActionEdit()
{
//    auto items = m_ui.mListWidget->selectedItems();
//	if (items.isEmpty())
//		return;

//	auto item = items.first();
//	uivec2 fid(item->data(VIEW_WIDGET_ITEM_PLUG).toUInt(), item->data(VIEW_WIDGET_ITEM_ENT).toUInt());
//	nsentity * ent = nse.resource<nsentity>(fid);
}

void ObjectView::onSelectionChanged()
{
//    QListWidgetItem * item = NULL;
//    auto items = m_ui.mListWidget->selectedItems();
//    if (!items.isEmpty())
//        item = items[0];

//    if (item == NULL)
//    {
//        nse.system<nsbuild_system>()->set_object_build_ent(NULL);
//        return;
//    }

//    uivec2 itid(item->data(VIEW_WIDGET_ITEM_PLUG).toUInt(), item->data(VIEW_WIDGET_ITEM_ENT).toUInt());
//    nsentity * ent = nse.resource<nsentity>(itid);
//    nse.system<nsbuild_system>()->set_object_build_ent(ent);
}
