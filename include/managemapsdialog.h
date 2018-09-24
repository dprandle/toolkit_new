#ifndef MANAGEMAPSDIALOG_H
#define MANAGEMAPSDIALOG_H

// Inlcudes
#include <ui_manage_maps_dialog.h>
#include <ui_editmapdialog.h>

class NewMapDialog;

class ManageMapsDialog : public QDialog
{
	Q_OBJECT
public:
	ManageMapsDialog(QWidget * parent = NULL);
	~ManageMapsDialog();

    void init();

	public slots:

	/*
	When a item is selected we want to display the maps information in the appropriate fields
	*/
	void onMapItemChanged(QTreeWidgetItem*, int);
	void onMapItemPressed(QTreeWidgetItem*, int);
	void onNewMap();
	void onDeleteMap();
	void onEditMap();
	void onLoad();
	void onCancel();
	void onEditColClick();

private:
	void _rescanPluginMaps();
	Ui::EditMapDialog mEditMapUI;
	Ui_ManageMapsDialog mUI;
	QDialog mEditD;
	NewMapDialog * mNewMapD;
	QColor prevC;
};
#endif
