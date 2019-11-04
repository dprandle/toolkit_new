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

#include <urho_common.h>
#include <urho_map_editor.h>

#include <mtdebug_print.h>
#include <ui_toolkit.h>
#include <toolkit.h>

Toolkit * Toolkit::toolkit_ptr_ = nullptr;

Toolkit::Toolkit(QWidget * parent)
    : QMainWindow(parent),
      ui(new Ui::Toolkit),
      init_(false),
      toolkit_context_(new Context()),
      game_context_(new Context())
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
}

Toolkit::~Toolkit()
{
    delete ui;
    //	m_urho.release();
}

void Toolkit::init()
{
    ui->map_editor->init(toolkit_context_);
    emit urho_init_complete();
    init_ = true;
    QTimer::singleShot(100, this, &Toolkit::on_actionLoad_View_triggered);
}

bool Toolkit::is_init()
{
    return init_;
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

Context * Toolkit::get_urho_toolkit_context()
{
    return toolkit_context_;
}

Toolkit & Toolkit::inst()
{
    return *toolkit_ptr_;
}
