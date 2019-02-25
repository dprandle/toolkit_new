#pragma once
#include <QMainWindow>

namespace Ui {
class Game_View;
}

class Game_View : public QMainWindow
{
   Q_OBJECT
   
  public:
   
   explicit Game_View(QWidget *parent = 0);
   ~Game_View();
   
  private:
   Ui::Game_View * ui;
};