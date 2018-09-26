#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QDockWidget>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSpinBox>
#include <QTimer>
#include <QToolButton>
#include <QWidgetAction>


#include <addnewbrushdialog.h>
#include <brushmenuwidget.h>
#include <camerasettingsdialog.h>
#include <managemapsdialog.h>
#include <managepluginsdialog.h>
#include <scene_view.h>
#include <toolkit.h>


//#include <entityeditordialog.h>
//#include <resource_browser.h>
//#include <resource_dialog.h>
//#include <resource_dialog_prev.h>
//#include <resource_dialog_prev_lighting.h>

Toolkit::Toolkit(Urho3D::Context* urho_context, QWidget* parent)
    : QMainWindow(parent)
    , m_brush_height(new QSpinBox())
    , m_current_layer(new QSpinBox())
    , m_layer_CB(new QComboBox())
    , m_brush_menu(new QMenu(this))
    , m_brush_tool_btn(new QToolButton())
    , m_brush_menu_widget(new BrushMenuWidget(this))
    , m_cam_settings(new CameraSettingsDialog(this))
    ,
    //m_resource_browser(new resource_browser(this)),
    m_grid_x(new QSpinBox())
    , m_grid_y(new QSpinBox())
    ,
    //m_res_dialog(new resource_dialog(this)),
    //m_res_dialog_prev(new resource_dialog_prev(this)),
    //m_res_dialog_prev_lighting(new resource_dialog_prev_lighting(this)),
    m_layers_above_hidden(0)
    , m_spinbox_val(0)
    , m_prev_layer_text(LAYER_ABOVE_TEXT)
{
    m_ui.setupUi(this);
    setWindowIcon(QIcon(":/Toolkit/bbicon_256px.ico"));
    m_ptr = this;
    m_ui.m_scene_view = new Scene_View(urho_context, this);
    setCentralWidget(m_ui.m_scene_view);

    QWidgetAction* wA = new QWidgetAction(this);
    wA->setDefaultWidget(m_brush_menu_widget);
    m_brush_menu->addAction(wA);

    m_brush_tool_btn->setPopupMode(QToolButton::MenuButtonPopup);
    m_brush_tool_btn->setMenu(m_brush_menu);

    m_brush_height->setRange(1, 16);

    m_layer_CB->addItem(LAYER_ABOVE_TEXT);
    m_layer_CB->addItem(LAYER_BELOW_TEXT);
    m_layer_CB->addItem(LAYER_ALL_TEXT);
    m_layer_CB->addItem(LAYER_ONLY_TEXT);

    m_grid_x->setRange(-256, 256);
    m_grid_y->setRange(-256, 256);
    m_grid_x->setValue(0);
    m_grid_y->setValue(0);

    m_ui.mTopToolbar->insertWidget(m_ui.actionUnlock, m_brush_tool_btn);
    m_ui.mTopToolbar->insertWidget(m_ui.actionUnlock, new QLabel("Height "));
    m_ui.mTopToolbar->insertWidget(m_ui.actionUnlock, m_brush_height);
    m_ui.mTopToolbar->insertWidget(m_ui.actionLayerMode, new QLabel("Hide "));
    m_ui.mTopToolbar->insertWidget(m_ui.actionLayerMode, m_layer_CB);
    m_ui.mTopToolbar->insertWidget(m_ui.actionLayerMode, new QLabel("Layer "));
    m_ui.mTopToolbar->insertWidget(m_ui.actionLayerMode, m_current_layer);
    m_ui.mTopToolbar->insertWidget(m_ui.actionMirrorMode, new QLabel("Mirror Center: "));
    m_ui.mTopToolbar->insertWidget(m_ui.actionMirrorMode, new QLabel("x"));
    m_ui.mTopToolbar->insertWidget(m_ui.actionMirrorMode, m_grid_x);
    m_ui.mTopToolbar->insertWidget(m_ui.actionMirrorMode, new QLabel("y"));
    m_ui.mTopToolbar->insertWidget(m_ui.actionMirrorMode, m_grid_y);

    QAction* deb = new QAction(QIcon(":/VecTB/icons/toolbars/Debug_Icon.png"), "Debug View", this);
    deb->setCheckable(true);
    m_ui.mTopToolbar->addAction(deb);
    connect(deb, SIGNAL(toggled(bool)), this, SLOT(on_debug_view(bool)));

    QAction* deb_spaces = new QAction(QIcon(":/VecTB/icons/toolbars/occupied_grid.png"), "Show Map Occupied Spaces", this);
    deb_spaces->setCheckable(true);
    m_ui.mTopToolbar->addAction(deb_spaces);
    connect(deb_spaces, SIGNAL(toggled(bool)), this, SLOT(on_view_occupied_spaces(bool)));

    m_current_layer->setRange(-256, 256);

    connect(m_brush_height, SIGNAL(valueChanged(int)), this, SLOT(on_brush_height_change(int)));
    connect(m_current_layer, SIGNAL(valueChanged(int)), this, SLOT(on_change_layer(int)));
    connect(m_layer_CB, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(on_layer_index_change(const QString&)));
    connect(m_grid_x, SIGNAL(valueChanged(int)), this, SLOT(on_mirror_center_change()));
    connect(m_grid_y, SIGNAL(valueChanged(int)), this, SLOT(on_mirror_center_change()));

    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

    //on_actionLoad_triggered();

    showMaximized();
    m_ui.m_scene_view->setFocus();
}

Toolkit::~Toolkit()
{
    //	m_urho.release();
}

void Toolkit::init()
{
    m_ui.mOutputView->init();
    m_ui.mTileView->init();
    m_ui.mObjectView->init();
    m_brush_menu_widget->init();
    scene_view()->init();
}

Scene_View* Toolkit::scene_view()
{
    return static_cast<Scene_View*>(m_ui.m_scene_view);
}

int message_box(QWidget* parent, const QString& title, const QString& msg, int buttons, int icon)
{
    QMessageBox mb(parent);
    mb.setIcon(QMessageBox::Icon(icon));
    mb.setWindowTitle(title);
    mb.setText(msg);
    mb.setStandardButtons(QMessageBox::StandardButton(buttons));
    return mb.exec();
}

OutputView* Toolkit::output_view()
{
    return m_ui.mOutputView;
}

//void Toolkit::load_plugin_files(const QDir & startingDir)
//{
//    if (startingDir.exists())
//    {
//        foreach(QFileInfo info, startingDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs))
//        {
//            if (info.isFile())
//                nse.load_plugin(info.absoluteFilePath().toStdString());
//            if (info.isDir())
//                load_plugin_files(info.dir());
//        }
//    }
//}

//resource_browser * Toolkit::res_browser()
//{
//    return m_resource_browser;
//}

void Toolkit::on_view_occupied_spaces(bool draw_)
{
    //    nse.system<nsselection_system>()->enable_draw_occupied_grid(draw_);
}

void Toolkit::on_actionCameraSettings_triggered()
{
    m_cam_settings->show();
}

CameraSettingsDialog* Toolkit::camera_settings()
{
    return m_cam_settings;
}

void Toolkit::closeEvent(QCloseEvent* pEvent)
{
    //    nsplugin * activePlug = nse.active();

    //    if (activePlug != NULL)
    //    {
    //        QMessageBox mb(QMessageBox::Warning, "Unsaved Changed", "There are unsaved changes. Would you like to save before exiting?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this);
    //        int ret = mb.exec();
    //        if (ret == QMessageBox::Cancel)
    //        {
    //            pEvent->ignore();
    //            return;
    //        }
    //        bool saveChanges = (ret == QMessageBox::Yes);
    //        if (saveChanges)
    //            on_actionSave_triggered();
    //        pEvent->accept();
    //    }
}

// resource_dialog * Toolkit::res_dialog()
// {
//     return m_res_dialog;
// }

// resource_dialog_prev * Toolkit::res_dialog_prev()
// {
//     return m_res_dialog_prev;
// }

// resource_dialog_prev_lighting * Toolkit::res_dialog_prev_lighting()
// {
//     return m_res_dialog_prev_lighting;
// }

/*!
Update call which happens every frame
If there is a slow down be sure to comment this out and check if the slowdown still occurs - I have found that this function
is very sensative and things like posting to the status bar here will significantly slow program operations
*/
void Toolkit::update_ui()
{
    //    bool chk = m_ui.actionEraserBrush->isChecked() || m_ui.actionObjectBrush->isChecked() || m_ui.actionTileBrush->isChecked();
    //    m_ui.actionNewBrush->setEnabled(nse.system<nsselection_system>()->valid_brush() && !chk);
    //    m_ui.actionSwapTiles->setEnabled(nse.system<nsselection_system>()->valid_tile_swap() && !chk && !m_ui.mTileView->ui()->mListWidget->selectedItems().isEmpty());
    //    m_ui.actionDeleteSelection->setEnabled(!nse.system<nsselection_system>()->empty() && !chk);

    //    m_current_layer->setEnabled(m_ui.actionLayerMode->isChecked() || m_ui.actionEraserBrush->isChecked() || m_ui.actionObjectBrush->isChecked() || m_ui.actionTileBrush->isChecked());

    //    nsscene * curScene = nse.current_scene();
    //    nsplugin * activePlug = nse.active();
    //    QString winTitle = "Build and Battle Toolkit [Active Plugin: ";

    //    if (activePlug != NULL)
    //    {
    //        winTitle += QString(activePlug->name().c_str());
    //    }

    //    winTitle += "] [Scene: ";

    //    m_ui.mCamToolbar->setEnabled(curScene != NULL);
    //    if (curScene != NULL)
    //    {
    //        winTitle += QString(curScene->name().c_str()) + ".map";

    //        uivec3 selObj = nse.system<nsselection_system>()->center();
    //        nsentity * ent = curScene->entity(selObj.xy());

    //        if (ent != NULL && (!m_ui.actionLayerMode->isChecked()||nse.system<nsbuild_system>()->enabled()))
    //        {
    //            nstform_comp * tComp = ent->get<nstform_comp>();
    //            if (tComp != NULL)
    //            {
    //                int32 layer = nstile_grid::grid(tComp->wpos(selObj.z)).z;
    //                int32 tmpVal = m_spinbox_val;
    //                m_current_layer->blockSignals(true);
    //                m_current_layer->setValue(layer*-1);
    //                m_current_layer->blockSignals(false);
    //                m_spinbox_val = layer*-1;
    //                if (m_ui.actionLayerMode->isChecked() && tmpVal != m_spinbox_val)
    //                    on_actionLayerMode_toggled(true);
    //            }
    //        }
    //      nse.system<nsbuild_system>()->set_layer(m_spinbox_val*-1);
    //    }

    //    winTitle += "]";
    //    if (winTitle != windowTitle())
    //        setWindowTitle(winTitle);
}

void Toolkit::_disable_side_tb_actions()
{
    m_ui.actionSelect->setChecked(false);
    m_ui.actionSelectArea->setChecked(false);
    m_ui.actionTileBrush->setChecked(false);
    m_ui.actionObjectBrush->setChecked(false);
    m_ui.actionEraserBrush->setChecked(false);
}

void Toolkit::on_actionSelect_triggered(bool pVal)
{
    if (pVal) {
        _disable_side_tb_actions();
        //        nse.system<nsbuild_system>()->enable(false);
    }
    m_ui.actionSelect->setChecked(true);
    m_ui.m_scene_view->setFocus();
}

void Toolkit::on_actionSelectArea_triggered(bool)
{
    output_view()->writeToScreen("This behaviour has not yet been implemented");
}

void Toolkit::on_actionToggleLighting_toggled(bool pChange)
{
    //nse.system<nsrender_system>()->enable_lighting(pChange);
}

void Toolkit::on_actionTileBrush_triggered(bool pVal)
{
    if (pVal) {
        _disable_side_tb_actions();

        //        nse.system<nsbuild_system>()->set_brush_type(nsbuild_system::brush_tile);
        //        nse.system<nsbuild_system>()->set_mode(nsbuild_system::build_mode);
        //        nse.system<nsbuild_system>()->set_active_brush_color(fvec4(0.0f, 1.0f, 0.0f, 0.8f));
        // Selecting item in tile view should set the current build tile
        //        nse.system<nsbuild_system>()->enable(true);
    }
    m_ui.actionTileBrush->setChecked(true);
    m_ui.m_scene_view->setFocus();
}

void Toolkit::on_change_layer(int pVal)
{
    bool toggle = m_ui.actionLayerMode->isChecked();

    if (toggle)
        on_actionLayerMode_toggled(false);

    int delta = pVal - m_spinbox_val;
    m_spinbox_val = pVal;

    //    if (nse.system<nsbuild_system>()->enabled())
    //        nse.system<nsselection_system>()->change_layer(delta);

    m_ui.m_scene_view->setFocus();

    if (toggle)
        on_actionLayerMode_toggled(toggle);
}

void Toolkit::on_actionObjectBrush_triggered(bool pVal)
{
    if (pVal) {
        _disable_side_tb_actions();

        // nse.system<nsbuild_system>()->set_brush_type(nsbuild_system::brush_object);
        // nse.system<nsbuild_system>()->set_mode(nsbuild_system::build_mode);
        // nse.system<nsbuild_system>()->set_active_brush_color(fvec4(0.0f, 1.0f, 0.0f, 0.8f));
        // nse.system<nsbuild_system>()->enable(true);
    }
    m_ui.actionObjectBrush->setChecked(true);
    m_ui.m_scene_view->setFocus();
}

void Toolkit::on_actionEraserBrush_triggered(bool pVal)
{
    if (pVal) {
        _disable_side_tb_actions();

        // nse.system<nsbuild_system>()->set_brush_type(nsbuild_system::brush_tile);
        // nse.system<nsbuild_system>()->set_mode(nsbuild_system::erase_mode);
        // nse.system<nsbuild_system>()->set_active_brush_color(fvec4(1.0f, 0.0f, 0.0f, 0.8f));
        // nse.system<nsbuild_system>()->enable(true);
    }
    m_ui.actionEraserBrush->setChecked(true);
    m_ui.m_scene_view->setFocus();
}

void Toolkit::on_actionMirrorMode_toggled(bool pEnable)
{
    // nse.system<nsselection_system>()->enable_mirror_selection(pEnable);
    // nse.system<nsbuild_system>()->enable_mirror(pEnable);
    //    m_ui.mMapView->setFocus();
}

void Toolkit::on_mirror_center_change()
{
    //    ivec3 grid(m_grid_x->value(), m_grid_y->value());
    //    fvec3 pos = nstile_grid::world(grid);
    //    nse.system<nsbuild_system>()->set_center(pos);
    //    m_ui.mMapView->setFocus();
}

void Toolkit::on_actionNew_triggered()
{
    //    m_ui.mMapView->setFocus();
}

void Toolkit::on_actionLoad_triggered()
{
    if (!m_ui.actionSelect->isChecked())
        on_actionSelect_triggered(true);

    ManagePluginsDialog mPlugs(this);
    mPlugs.init();

    if (mPlugs.exec() == QDialog::Accepted) {
    }
    m_ui.m_scene_view->setFocus();
}

void Toolkit::on_brush_height_change(int pHeight)
{
    // nsentity * ent = nse.system<nsbuild_system>()->tile_brush();
    // if (ent == NULL)
    //     return;

    // nstile_brush_comp * comp = ent->get<nstile_brush_comp>();
    // if (comp != NULL)
    //     comp->set_height(pHeight);

    // m_ui.mMapView->setFocus();
}

void Toolkit::on_actionShowAllHidden_toggled(bool pState)
{
    //    nsscene * scene = nse.current_scene();
    //    if (scene == NULL)
    //        return;

    //    auto ents = scene->entities();
    //    auto iter = ents.begin();
    //    while (iter != ents.end())
    //    {
    //        nstform_comp * tComp = (*iter)->get<nstform_comp>();
    //        tComp->set_hidden_state(nstform_comp::hide_none, pState);
    //        ++iter;
    //    }
}

void Toolkit::on_actionSwitchMap_triggered()
{
    //	ManageMapsDialog mMaps(this);
    //	mMaps.init(this);

    //	if (mMaps.exec() == QDialog::Accepted)
    //	{

    //	}
    //    m_ui.mMapView->setFocus();
}

void Toolkit::on_actionSave_triggered()
{
    //    nsbuild_system::BrushMode tmpBrushMode = nse.system<nsbuild_system>()->brushMode();
    //    nsbuild_system::Mode tmpMode = nse.system<nsbuild_system>()->mode();
    //    fvec4 tmpCol = nse.system<nsbuild_system>()->activeBrushColor();
    //    nsentity * tmpEnt = nse.system<nsbuild_system>()->buildent();

    //    bool toggle = m_ui.actionLayerMode->isChecked();
    //    int tmpCurLayer = m_spinbox_val;
    //	if (toggle)
    //        on_layer_mode(false);

    //    if (!m_ui.actionSelect->isChecked())
    //        nse.system<nsbuild_system>()->enable(false, fvec2());

    //    nse.savePlugin(nse.active(), true);

    //    if (!m_ui.actionSelect->isChecked())
    //	{
    //        nse.system<nsbuild_system>()->setBrushMode(tmpBrushMode);
    //        nse.system<nsbuild_system>()->setMode(tmpMode);
    //        nse.system<nsbuild_system>()->setActiveBrushColor(tmpCol);
    //        nse.system<nsbuild_system>()->setBuildEnt(tmpEnt);
    //        nse.system<nsbuild_system>()->enable(true, fvec2());
    //        m_spinbox_val = 0;
    //        on_change_layer(tmpCurLayer);
    //	}

    //	if (toggle)
    //        on_layer_mode(toggle);

    //    m_ui.mMapView->setFocus();
}

void Toolkit::on_actionUnlock_toggled(bool pVal)
{
    //    nse.system<nsbuild_system>()->enable_overwrite(!pVal);
    //    m_ui.mMapView->setFocus();
}

ObjectView* Toolkit::object_view()
{
    return m_ui.mObjectView;
}

TileView* Toolkit::tile_view()
{
    return m_ui.mTileView;
}

void Toolkit::on_actionSwapTiles_triggered()
{
    //    nsentity * newTile = nse.system<nsbuild_system>()->tile_build_ent();
    //	if (newTile == NULL)
    //		return;

    //    nse.system<nsselection_system>()->tile_swap(newTile);
    //    m_ui.mMapView->setFocus();
}

void Toolkit::on_actionResource_Browser_triggered()
{
    //    m_resource_browser->show();
}

void Toolkit::refresh_views()
{
    m_ui.mTileView->refresh();
    m_ui.mObjectView->refresh();
    //    m_brush_menu_widget->setupLW();
    update_brush_tool_button();
}

void Toolkit::on_debug_view(bool pNewVal)
{
    //    map()->make_current();
    //    nse.system<nsrender_system>()->enable_debug_draw(pNewVal);
}

void Toolkit::on_actionDeleteSelection_triggered()
{
    //    nse.system<nsselection_system>()->del();
    //    m_ui.mMapView->setFocus();
}

void Toolkit::on_actionNewBrush_triggered()
{
    AddNewBrushDialog brushD(this);

    brushD.init();
    if (brushD.exec() == QDialog::Accepted) {
    }
    m_ui.m_scene_view->setFocus();
}

void Toolkit::on_set_current_brush()
{
    //	if (pBrush == NULL)
    //		return;

    //    m_brush_menu_widget->setupLW();
    //    nse.system<nsbuild_system>()->setTileBrush(pBrush);

    //    if (m_ui.actionEraserBrush->isChecked())
    //        on_eraser_brush(true);
    //    else if (m_ui.actionTileBrush->isChecked())
    //        on_tile_brush(true);

    //	if (!pBrush->iconPath().empty())
    //        m_brush_menu->setIcon(QIcon(pBrush->iconPath().c_str()));

    //    m_brush_menu_widget->setSelectedItem(pBrush);

    //    nstile_brush_comp * comp = pBrush->get<nstile_brush_comp>();
    //	if (comp != NULL)
    //        comp->setHeight(m_brush_height->value());
    //    m_ui.mMapView->setFocus();
}

void Toolkit::on_brush_change(QListWidgetItem*)
{
    //	if (pCurrent == NULL)
    //	{
    //        m_brush_menu->setIcon(QIcon());
    //		return;
    //	}

    //	uivec2 id(pCurrent->data(VIEW_WIDGET_ITEM_PLUG).toUInt(), pCurrent->data(VIEW_WIDGET_ITEM_ENT).toUInt());
    //    nsentity * ent = nse.resource<nsentity>(id);
    //	if (ent == NULL)
    //	{
    //		QMessageBox mb(QMessageBox::Warning, "Error in brush", "The brush with name " + pCurrent->text() + " can not be found for some reason...", QMessageBox::NoButton, this);
    //		mb.exec();
    //		return;
    //	}

    //    nse.system<nsbuild_system>()->setTileBrush(ent);
    //    nstile_brush_comp * comp = ent->get<nstile_brush_comp>();
    //	if (comp != NULL)
    //        comp->setHeight(m_brush_height->value());

    //    if (m_ui.actionEraserBrush->isChecked())
    //        on_eraser_brush(true);
    //    else if (m_ui.actionTileBrush->isChecked())
    //        on_tile_brush(true);

    //    m_brush_menu->setIcon(pCurrent->icon());
    //    m_ui.mMapView->setFocus();
}

void Toolkit::on_layer_index_change(const QString& pItem)
{
    if (m_ui.actionLayerMode->isChecked()) {
        m_layer_CB->blockSignals(true);
        m_layer_CB->setCurrentText(m_prev_layer_text);
        on_actionLayerMode_toggled(false);
        m_layer_CB->setCurrentText(pItem);
        m_prev_layer_text = pItem;
        m_layer_CB->blockSignals(false);
        on_actionLayerMode_toggled(true);
    }
}

void Toolkit::on_actionLayerMode_toggled(bool pVal)
{
    //    nsscene * scene = nse.current_scene();
    //    if (scene == NULL)
    //        return;

    //    int val = m_current_layer->value();
    //    if (pVal)
    //        m_layers_above_hidden = val;
    //    else
    //    {
    //        val = m_layers_above_hidden;
    //        m_layers_above_hidden = 0;
    //    }

    //    if (m_layer_CB->currentText() == LAYER_ABOVE_TEXT)
    //        scene->hide_layers_above(val, pVal);
    //    else if (m_layer_CB->currentText() == LAYER_BELOW_TEXT)
    //        scene->hide_layers_below(val, pVal);
    //    else if (m_layer_CB->currentText() == LAYER_ALL_TEXT)
    //    {
    //        scene->hide_layers_above(val, pVal);
    //        scene->hide_layers_below(val, pVal);
    //    }
    //    else
    //        scene->hide_layer(val, pVal);

    //    m_ui.mMapView->setFocus();
}

void Toolkit::on_brush_double_click()
{
    m_brush_menu->hide();
    m_ui.m_scene_view->setFocus();
}

void Toolkit::on_actionStampMode_toggled(bool pVal)
{
    //    nse.system<nsbuild_system>()->enable_stamp_mode(pVal);
}

void Toolkit::on_actionHideSelection_triggered()
{
    //nse.system<nsselection_system>()->set_hidden_state(nstform_comp::hide_object, true);
}

void Toolkit::on_actionUnhideSelection_triggered()
{
    // nse.system<nsselection_system>()->set_hidden_state(nstform_comp::hide_object, false);
}

void Toolkit::on_actionCameraCenter_triggered(bool pVal)
{
    if (pVal) {
        m_ui.actionObjectCenter->setChecked(!pVal);

        //        nse.system<nscamera_system>()->set_mode(nscamera_system::mode_free);
    }
    m_ui.actionCameraCenter->setChecked(true);
    m_ui.m_scene_view->setFocus();
}

void Toolkit::on_actionFrontView_triggered()
{
    //    nse.system<nscamera_system>()->set_view(nscamera_system::view_front_0);
}

void Toolkit::on_actionIsoView_triggered()
{
    //    nse.system<nscamera_system>()->set_view(nscamera_system::view_iso_0);
}

void Toolkit::on_actionTopDown_triggered()
{
    //    nse.system<nscamera_system>()->set_view(nscamera_system::view_top_0);
}

void Toolkit::on_actionObjectCenter_triggered(bool pVal)
{
    if (pVal) {
        m_ui.actionCameraCenter->setChecked(!pVal);
        //        nse.system<nscamera_system>()->set_mode(nscamera_system::mode_focus);
    }
    m_ui.actionObjectCenter->setChecked(true);
    m_ui.m_scene_view->setFocus();
}

void Toolkit::update_brush_tool_button()
{
    // nsentity * ent = nse.system<nsbuild_system>()->tile_brush();
    // if (ent == NULL)
    // {
    //     m_brush_tool_btn->setIcon(QIcon());
    //     return;
    // }

    // QString icon_path = ent->icon_path().c_str();
    // if (!icon_path.isEmpty())
    //     m_brush_tool_btn->setIcon(QIcon(icon_path));
    // else
    //     m_brush_tool_btn->setIcon(QIcon(":/ResourceIcons/icons/default_brush.png"));
}

void Toolkit::on_actionRedo_triggered()
{
    //    m_ui.mMapView->setFocus();
}

void Toolkit::on_actionUndo_triggered()
{
    //    m_ui.mMapView->setFocus();
}

Toolkit* Toolkit::m_ptr = nullptr;

Toolkit& Toolkit::inst()
{
    return *m_ptr;
}
