#include <mtdebug_print.h>
#include <ui_vector2d_int_editor.h>
#include <urho_attrib_edit_widget.h>

Urho_Vec2D_Int_Ui::Urho_Vec2D_Int_Ui() : ui(new Ui::Vector2D_Int_Editor)
{}

Urho_Vec2D_Int_Ui::~Urho_Vec2D_Int_Ui()
{
    delete ui;
}

void Urho_Vec2D_Int_Ui::setup_ui(QWidget * widget)
{
    ui->setupUi(widget);
}

Attrib_Edit_Widget::Attrib_Edit_Widget(Urho3D::VariantType type, QWidget * parent) : QWidget(parent)
{
    create_ui(type);
    ui->setup_ui(this);
}

Attrib_Edit_Widget::~Attrib_Edit_Widget()
{}

void Attrib_Edit_Widget::create_ui(Urho3D::VariantType type)
{
    switch (type)
    {
    case (Urho3D::VAR_INTVECTOR2):
        dout << "Recognize vec 2 int";        
        ui = new Urho_Vec2D_Int_Ui;
        break;
    default:
        break;
    }
}
