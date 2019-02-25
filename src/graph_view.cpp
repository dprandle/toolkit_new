#include <graph_view.h>
#include <ui_graph_view.h>

Graph_View::Graph_View(QWidget *parent):
QMainWindow(parent),
ui(new Ui::Graph_View)
{
   ui->setupUi(this);
}

Graph_View::~Graph_View()
{
    delete ui;
}