#pragma once
#include <QMainWindow>

#include <Urho3D/Core/Variant.h>
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

   void clear_selection();

   void set_selected_data(const Urho3D::VariantVector & data, const Urho3D::StringHash & type);

   void update_node();

   public slots:

   void urho_init_complete();

  private:
   Ui::Details_View * ui;
   
};