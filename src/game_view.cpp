#include <game_view.h>
#include <ui_game_view.h>

Game_View::Game_View(QWidget *parent):
QMainWindow(parent),
ui(new Ui::Game_View)
{
   ui->setupUi(this);
}

Game_View::~Game_View()
{
    delete ui;
}