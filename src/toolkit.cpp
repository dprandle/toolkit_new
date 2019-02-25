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

#include <Urho3D/Engine/Application.h>

#include <ui_toolkit.h>
#include <toolkit.h>

Toolkit * Toolkit::toolkit_ptr_ = nullptr;

Toolkit::Toolkit(QWidget * parent)
    : QMainWindow(parent),
    ui(new Ui::Toolkit),
      toolkit_context_(new Urho3D::Context()),
      game_context_(new Urho3D::Context())
{
    toolkit_ptr_ = this;

    ui->setupUi(this);
    setWindowIcon(QIcon(":/Toolkit/bbicon_256px.ico"));

    setCentralWidget(nullptr);
    showMaximized();
}

Toolkit::~Toolkit()
{
    delete ui;
    //	m_urho.release();
}

void Toolkit::init()
{
    ui->map_editor->init(toolkit_context_);

    remove_dock_widgets();
    
    ui->dockWidget_details->show();
    ui->dockWidget_graph->show();
    ui->dockWidget_prefab_view->show();
    ui->dockWidget_map_editor->show();
    ui->dockWidget_game_view->show();
    ui->dockWidget_assets->show();
    ui->dockWidget_console->show();


    addDockWidget(Qt::LeftDockWidgetArea, ui->dockWidget_prefab_view);
    addDockWidget(Qt::LeftDockWidgetArea, ui->dockWidget_graph);

    splitDockWidget(ui->dockWidget_graph, ui->dockWidget_prefab_view, Qt::Vertical);


    addDockWidget(Qt::LeftDockWidgetArea, ui->dockWidget_assets);
    tabifyDockWidget(ui->dockWidget_prefab_view, ui->dockWidget_assets);

    addDockWidget(Qt::LeftDockWidgetArea, ui->dockWidget_console);
    tabifyDockWidget(ui->dockWidget_assets, ui->dockWidget_console);


    addDockWidget(Qt::LeftDockWidgetArea, ui->dockWidget_map_editor);
    splitDockWidget(ui->dockWidget_graph, ui->dockWidget_map_editor, Qt::Horizontal);

    addDockWidget(Qt::LeftDockWidgetArea, ui->dockWidget_game_view);
    tabifyDockWidget(ui->dockWidget_map_editor, ui->dockWidget_game_view);

    addDockWidget(Qt::RightDockWidgetArea, ui->dockWidget_details);

    resizeDocks({ui->dockWidget_console, ui->dockWidget_details}, {1700,200}, Qt::Horizontal);
}

void Toolkit::remove_dock_widgets()
{
    removeDockWidget(ui->dockWidget_map_editor);
    removeDockWidget(ui->dockWidget_assets);
    removeDockWidget(ui->dockWidget_game_view);
    removeDockWidget(ui->dockWidget_prefab_view);
    removeDockWidget(ui->dockWidget_console);
    removeDockWidget(ui->dockWidget_details);
    removeDockWidget(ui->dockWidget_graph);
}

Toolkit & Toolkit::inst()
{
    return *toolkit_ptr_;
}
