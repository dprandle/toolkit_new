#include <QSettings>
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

#include <mtdebug_print.h>
#include <ui_toolkit.h>
#include <toolkit.h>

Toolkit * Toolkit::toolkit_ptr_ = nullptr;

Toolkit::Toolkit(QWidget * parent)
    : QMainWindow(parent),
    ui(new Ui::Toolkit),
      toolkit_context_(new Urho3D::Context()),
      game_context_(new Urho3D::Context())
{
    QCoreApplication::setOrganizationName("Earth Banana Games");
    QCoreApplication::setOrganizationDomain("earthbanana.com");
    QCoreApplication::setApplicationName("Build and Battle");
    QCoreApplication::setApplicationVersion("1.0.0");

    toolkit_ptr_ = this;

    ui->setupUi(this);
    setWindowIcon(QIcon(":/Toolkit/bbicon_256px.ico"));

    setCentralWidget(nullptr);
    showMaximized();

    connect(ui->dockWidget_map_editor, &QDockWidget::topLevelChanged, this, &Toolkit::dock_widget_floating_changed);
    connect(ui->dockWidget_assets, &QDockWidget::topLevelChanged, this, &Toolkit::dock_widget_floating_changed);
    connect(ui->dockWidget_game_view, &QDockWidget::topLevelChanged, this, &Toolkit::dock_widget_floating_changed);
    connect(ui->dockWidget_prefab_view, &QDockWidget::topLevelChanged, this, &Toolkit::dock_widget_floating_changed);
    connect(ui->dockWidget_console, &QDockWidget::topLevelChanged, this, &Toolkit::dock_widget_floating_changed);
    connect(ui->dockWidget_details, &QDockWidget::topLevelChanged, this, &Toolkit::dock_widget_floating_changed);
    connect(ui->dockWidget_graph, &QDockWidget::topLevelChanged, this, &Toolkit::dock_widget_floating_changed);

    connect(ui->dockWidget_map_editor, &QDockWidget::dockLocationChanged, this, &Toolkit::dock_widget_area_changed);
    connect(ui->dockWidget_assets, &QDockWidget::dockLocationChanged, this, &Toolkit::dock_widget_area_changed);
    connect(ui->dockWidget_game_view, &QDockWidget::dockLocationChanged, this, &Toolkit::dock_widget_area_changed);
    connect(ui->dockWidget_prefab_view, &QDockWidget::dockLocationChanged, this, &Toolkit::dock_widget_area_changed);
    connect(ui->dockWidget_console, &QDockWidget::dockLocationChanged, this, &Toolkit::dock_widget_area_changed);
    connect(ui->dockWidget_details, &QDockWidget::dockLocationChanged, this, &Toolkit::dock_widget_area_changed);
    connect(ui->dockWidget_graph, &QDockWidget::dockLocationChanged, this, &Toolkit::dock_widget_area_changed);

}

Toolkit::~Toolkit()
{
    delete ui;
    //	m_urho.release();
}

void Toolkit::init()
{
    ui->map_editor->init(toolkit_context_);
    QTimer::singleShot(10,this,&Toolkit::on_actionLoad_View_triggered);
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

void Toolkit::on_actionSave_View_triggered()
{
    QSettings settings;
    settings.setValue("Toolkit_UI", saveState(1.0));
}

void Toolkit::on_actionLoad_View_triggered()
{
    QSettings settings;
    restoreState(settings.value("Toolkit_UI").toByteArray(), 1.0);
}

void Toolkit::dock_widget_floating_changed(bool floating)
{}

void Toolkit::dock_widget_area_changed(Qt::DockWidgetArea area)
{}

Toolkit & Toolkit::inst()
{
    return *toolkit_ptr_;
}
