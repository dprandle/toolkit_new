#include <prefab_view.h>
#include <ui_prefab_view.h>

Prefab_View::Prefab_View(QWidget *parent):
QMainWindow(parent),
ui(new Ui::Prefab_View)
{
   ui->setupUi(this);
}

Prefab_View::~Prefab_View()
{
    delete ui;
}