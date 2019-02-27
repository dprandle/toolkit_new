#include <details_view.h>
#include <ui_details_view.h>

Details_View::Details_View(QWidget * parent) : QMainWindow(parent), ui(new Ui::Details_View)
{
    ui->setupUi(this);
}

void Details_View::set_node(Urho3D::Node * nd)
{
    ui->comp_widget->setup_ui(nd);
}

Details_View::~Details_View()
{
    delete ui;
}

void Details_View::update_node()
{
    ui->comp_widget->update_tw_from_node();
}
