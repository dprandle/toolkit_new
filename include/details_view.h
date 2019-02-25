#pragma once
#include <QMainWindow>

namespace Ui {
class Details_View;
}

namespace Urho3D {
   class Node;
}

class Details_View : public QMainWindow
{
   Q_OBJECT
   
  public:
   
   explicit Details_View(QWidget *parent = 0);
   ~Details_View();

   void set_node(Urho3D::Node * nd);
   
  private:
   Ui::Details_View * ui;
};