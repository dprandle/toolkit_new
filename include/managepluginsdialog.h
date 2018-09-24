#ifndef MANAGEPLUGINSDIALOG_H
#define MANAGEPLUGINSDIALOG_H

// Inlcudes
#include <ui_manage_plugins_dialog.h>
#include <ui_newplugindialog.h>
#include <QVector>
#include <QColor>

//void AddParents(nsplugin * pCurPlug, QVector<nsplugin*> & pPlugsToLoad, std::set<nsstring> & pUnloadedPlugs, nsplugin_manager * pPlugs);

class ManagePluginsDialog : public QDialog
{
	Q_OBJECT

public:
	ManagePluginsDialog(QWidget * parent = NULL);
	~ManagePluginsDialog();

    void init();
	void addPluginsToTreeWidget();

	/*! 
	Recursive function which will check all parents of the plugin before checking the plugin
	*/
	void check(QTreeWidgetItem * pItem);

	/*!
	Recursive function which will uncheck all children of the plugin before unchecking the plugin
	*/
	void uncheck(QTreeWidgetItem * pItem);

	public slots:

	void onNewPlugin();
	void onDeletePlugin();
	void onSelectionChange();
	void onSetActive();
	void onDetails();
	void onEditPlugin();
	void onNewPluginAccept();
	void onNewPluginReject();
	void onOkay();
	void onCancel();
	void onItemChanged(QTreeWidgetItem*,int);
	void onColorChange();

private:
    bool _itemHasCheckedChildren(QTreeWidgetItem* item);
	void _setColorStyle();

	Ui_ManagePluginsDialog mUI;
	QDialog * mNewPlugDialog;
	Ui_NewPluginDialog mNewPlugUI;
	Qt::CheckState mPressedState;
	QString mInitialActive;
	QColor prevC;
};
#endif
