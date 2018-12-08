#include <prefab_editor_dialog.h>
#include <prefab_editor.h>
#include <mtdebug_print.h>
#include <QCloseEvent>

Prefab_Editor_Dialog::Prefab_Editor_Dialog(QWidget * parent) : QDialog(parent)
{
    ui_.setupUi(this);
}

Prefab_Editor_Dialog::~Prefab_Editor_Dialog()
{}

void Prefab_Editor_Dialog::init(Urho3D::Context * context)
{
    ui_.prefab_view->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    ui_.prefab_view->setMinimumSize(QSize(400, 400));
    ui_.prefab_view->init_editor<Prefab_Editor>(context);
    auto editor = ui_.prefab_view->get_editor<Prefab_Editor>();
    ui_.comp_widget->setup_ui(editor->dir_light_node_);
}

void Prefab_Editor_Dialog::closeEvent(QCloseEvent * ev)
{
    hide();
    ev->ignore();
}
