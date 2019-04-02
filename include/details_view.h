#pragma once
#include <QMainWindow>

#include <Urho3D/Container/Vector.h>

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

   void set_nodes(const Urho3D::Vector<Urho3D::Node *> & nodes);

   void update_node();

   public slots:

   void urho_init_complete();

  private:
   Ui::Details_View * ui;
   
};