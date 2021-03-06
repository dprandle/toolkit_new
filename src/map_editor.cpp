#include <map_editor.h>
#include <ui_map_editor.h>
#include <urho_map_editor.h>

Map_Editor::Map_Editor(QWidget *parent):
QMainWindow(parent),
ui(new Ui::Map_Editor)
{
   ui->setupUi(this);
}

Map_Editor::~Map_Editor()
{
    delete ui;
}

Urho_Map_Editor * Map_Editor::get_urho_editor()
{
    return ui->scene_view->get_editor<Urho_Map_Editor>();
}

void Map_Editor::init(Urho3D::Context * ctxt)
{
    ui->scene_view->init_editor<Urho_Map_Editor>(ctxt);
}