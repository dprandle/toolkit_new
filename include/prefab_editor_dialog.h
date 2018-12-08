#pragma once

#include <ui_prefab_editor_dialog.h>

namespace Urho3D
{
class Context;
}

class Prefab_Editor_Dialog : public QDialog
{
    Q_OBJECT
  public:
    Prefab_Editor_Dialog(QWidget * parent = nullptr);
    ~Prefab_Editor_Dialog();

    void init(Urho3D::Context * context);

    void closeEvent(QCloseEvent * ev);

    Ui_Prefab_Editor_Dialog * get_ui() {return &ui_;}

  private:
    Ui_Prefab_Editor_Dialog ui_;
};