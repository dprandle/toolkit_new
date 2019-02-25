#pragma once
#include <QMainWindow>

namespace Ui {
class Graph_View;
}

class Graph_View : public QMainWindow
{
   Q_OBJECT
   
  public:
   
   explicit Graph_View(QWidget *parent = 0);
   ~Graph_View();
   
  private:
   Ui::Graph_View * ui;
};