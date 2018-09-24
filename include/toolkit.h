/*!
\file toolkit.h

\brief Header file for Toolkit class

This file contains all of the neccessary declarations for the Toolkit class.

\author Daniel Randle
\date December 11 2013
\copywrite Earth Banana Games 2013
*/

#ifndef TOOLKIT_H
#define TOOLKIT_H

#include <QMainWindow>
#include <ui_toolkit.h>
#include <QDir>
#include <QList>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Container/HashMap.h>

#define LAYER_ABOVE_TEXT "Above"
#define LAYER_BELOW_TEXT "Below"
#define LAYER_ALL_TEXT "All Except"
#define LAYER_ONLY_TEXT "Only"

// Main Window
#define MAINWIN_MAPVIEWTITLE_DOCK "Map View"
#define MAINWIN_OUTVIEWTITLE_DOCK "Output View"


#define mbox(title,msg) message_box(this, title, msg, QMessageBox::Ok, QMessageBox::Warning)
#define mboxb(title,msg,b) message_box(this, title, msg, b, QMessageBox::Warning)

#define mboxq(title,msg) message_box(this, title, msg, QMessageBox::Ok, QMessageBox::Question)
#define mboxqb(title,msg,b) message_box(this, title, msg, b, QMessageBox::Question)

#define mboxc(title,msg) message_box(this, title, msg, QMessageBox::Ok, QMessageBox::Critical)
#define mboxcb(title,msg,b) message_box(this, title, msg, b, QMessageBox::Critical)


class QMessageBox;
class QSpinBox;
class QCheckBox;
class QLabel;
class QMenu;
class QIcon;
class QToolButton;
class QComboBox;

class SceneView;
class InputMap;
class TileView;
class ObjectView;
class OutputView;
class BrushMenuWidget;
class CameraSettingsDialog;
//class resource_browser;
//class resource_dialog;
//class resource_dialog_prev;
//class resource_dialog_prev_lighting;


int message_box(QWidget * parent, const QString & title, const QString & msg, int buttons, int icon);

class Toolkit : public QMainWindow
{

	Q_OBJECT
	
public:
    Toolkit(Urho3D::Context * urho_context, QWidget *parent = 0);
	~Toolkit();

	void closeEvent(QCloseEvent *pEvent);

    OutputView * output_view();

    ObjectView * object_view();
    	
    TileView *  tile_view();

	SceneView * scene_view();

    CameraSettingsDialog * camera_settings();

	void init();

//    resource_browser * res_browser();

//    resource_dialog * res_dialog();

//    resource_dialog_prev * res_dialog_prev();

//    resource_dialog_prev_lighting * res_dialog_prev_lighting();

//    void load_plugin_files(const QDir & startingDir);

    void refresh_views();

    void update_brush_tool_button();

    static Toolkit & inst();

  public slots:

    void update_ui();

    void on_actionSelect_triggered(bool);
    void on_actionSelectArea_triggered(bool);
    void on_actionTileBrush_triggered(bool);
    void on_actionObjectBrush_triggered(bool);
    void on_actionEraserBrush_triggered(bool);
    void on_actionNew_triggered();
    void on_actionLoad_triggered();
    void on_actionSave_triggered();
    void on_actionUnlock_toggled(bool);
    void on_actionSwapTiles_triggered();
    void on_actionDeleteSelection_triggered();
    void on_actionNewBrush_triggered();
    void on_actionLayerMode_toggled(bool);
    void on_actionStampMode_toggled(bool);
    void on_actionHideSelection_triggered();
    void on_actionUnhideSelection_triggered();
    void on_actionCameraCenter_triggered(bool);
    void on_actionFrontView_triggered();
    void on_actionObjectCenter_triggered(bool);
    void on_actionIsoView_triggered();
    void on_actionTopDown_triggered();
    void on_actionRedo_triggered();
    void on_actionUndo_triggered();
    void on_actionSwitchMap_triggered();
    void on_actionCameraSettings_triggered();
    void on_actionShowAllHidden_toggled(bool);
    void on_actionMirrorMode_toggled(bool);
    void on_actionToggleLighting_toggled(bool);
    void on_actionResource_Browser_triggered();


    void on_mirror_center_change();
    void on_change_layer(int);
    void on_brush_height_change(int);
    void on_brush_double_click();
    void on_set_current_brush();
    void on_brush_change(QListWidgetItem*);
    void on_layer_index_change(const QString &);

    void on_debug_view(bool);
    void on_view_occupied_spaces(bool);

  private:

    void _disable_side_tb_actions();

	Ui_Toolkit m_ui;

    static Toolkit * m_ptr;

    QSpinBox * m_brush_height;
    QSpinBox * m_current_layer;
    QSpinBox * m_grid_x;
    QSpinBox * m_grid_y;
    QComboBox * m_layer_CB;
    QMenu * m_brush_menu;
    QToolButton * m_brush_tool_btn;

    BrushMenuWidget * m_brush_menu_widget;
    CameraSettingsDialog * m_cam_settings;

//    resource_browser * m_resource_browser;

//    resource_dialog * m_res_dialog;
//    resource_dialog_prev * m_res_dialog_prev;
//    resource_dialog_prev_lighting * m_res_dialog_prev_lighting;

    int m_layers_above_hidden;
    int m_spinbox_val;
    QString m_prev_layer_text;
};


#define bbtk Toolkit::inst()

#endif // TOOLKIT_H
