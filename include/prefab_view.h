#pragma once
#include <QMainWindow>

namespace Ui {
class Prefab_View;
}

class Prefab_View : public QMainWindow
{
   Q_OBJECT
   
  public:
   
   explicit Prefab_View(QWidget *parent = 0);
   ~Prefab_View();
   
  private:
   Ui::Prefab_View * ui;
};