#include <asset_view.h>
#include <ui_asset_view.h>

Asset_View::Asset_View(QWidget *parent):
QMainWindow(parent),
ui(new Ui::Asset_View)
{
   ui->setupUi(this);
}

Asset_View::~Asset_View()
{
    delete ui;
}