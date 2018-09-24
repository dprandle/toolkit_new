#include <QToolBar>
#include <QMessageBox>
#include <brushmenuwidget.h>
#include <addnewbrushdialog.h>
#include <toolkit.h>

BrushMenuWidget::BrushMenuWidget(QWidget * parent):
    QWidget(parent),
    mTB(new QToolBar())
{
	mUI.setupUi(this);
	mUI.verticalLayout->addWidget(mTB);
	mTB->addAction(mUI.actionNewBrush);
	mTB->addAction(mUI.actionEditBrush);
	mTB->addAction(mUI.actionDeleteBrush);
	mTB->setLayoutDirection(Qt::RightToLeft);
}

BrushMenuWidget::~BrushMenuWidget()
{

}

void BrushMenuWidget::init()
{
}

void BrushMenuWidget::setupLW()
{
//	QListWidgetItem * curItem = mUI.mBrushedLW->currentItem();
//	QString itemName;
//	if (curItem != NULL)
//		itemName = curItem->text();

//	mUI.mBrushedLW->clear();


//	auto plugiter = nse.plugins()->begin();
//	while (plugiter != nse.plugins()->end())
//	{
//		nsplugin * plg = nse.plugin(plugiter->first);

//		nsentity_manager * ents = plg->manager<nsentity_manager>();

//		auto iter = ents->begin();
//		while (iter != ents->end())
//		{
//			nsentity * curEnt = ents->get(iter->first);

//			if (curEnt->has<nstile_brush_comp>())
//			{
//				QListWidgetItem * item = new QListWidgetItem(curEnt->name().c_str());
//                item->setData(VIEW_WIDGET_ITEM_PLUG, curEnt->plugin_id());
//				item->setData(VIEW_WIDGET_ITEM_ENT, curEnt->id());

//				// make else case to give default icon in case no icon assigned
//                if (!curEnt->icon_path().empty())
//                    item->setIcon(QIcon(curEnt->icon_path().c_str()));
//                else
//                    item->setIcon(QIcon(":/ResourceIcons/icons/default_brush.png"));

//				mUI.mBrushedLW->addItem(item);
//			}
//			++iter;
//		}


//		++plugiter;
//	}

//	// Set the selected brush back to whatever was selected before remaking the brush list
//	auto fItems = mUI.mBrushedLW->findItems(itemName, Qt::MatchCaseSensitive);

//	if (!fItems.isEmpty() && fItems.first() != NULL)
//	{
//		mUI.mBrushedLW->setCurrentItem(fItems.first());
//		mUI.mBrushedLW->setItemSelected(fItems.first(),true);
//	}
//	else if (mUI.mBrushedLW->count() > 0)
//	{
//		QListWidgetItem * item = mUI.mBrushedLW->item(0);
//		mUI.mBrushedLW->setCurrentItem(item);
//		mUI.mBrushedLW->setItemSelected(item, true);
//	}
//	else
//	{
//		mUI.mBrushedLW->setCurrentItem(NULL);
//	}
}

void BrushMenuWidget::onDeleteBrush()
{
    QMessageBox mb(&bbtk);
	mb.setText("Are you sure you want to delete the brush?");
	if (mb.exec() == QMessageBox::Accepted)
	{

	}
}

void BrushMenuWidget::onEditBrush()
{

}

void BrushMenuWidget::onNewBrush()
{
    AddNewBrushDialog brushD(&bbtk);
    brushD.init();
	if (brushD.exec() == QDialog::Accepted)
	{

	}
}

void BrushMenuWidget::onDoubleClick()
{
    bbtk.update_brush_tool_button();
    bbtk.on_brush_double_click();
}

void BrushMenuWidget::onSelectionChanged()
{
    bbtk.update_brush_tool_button();
}
