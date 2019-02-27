#pragma once
#include <QMainWindow>

namespace Ui {
class Map_Editor;
}

namespace Urho3D {
   class Context;
}

class Map_Editor : public QMainWindow
{
   Q_OBJECT
   
  public:
   
   explicit Map_Editor(QWidget *parent = 0);
   ~Map_Editor();

   void init(Urho3D::Context * ctxt);

  private:
   Ui::Map_Editor * ui;
};