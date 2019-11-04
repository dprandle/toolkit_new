#pragma once

#include <QMainWindow>
#include <QMessageBox>
#include <QDir>
#include <QList>
#include <Urho3D/Core/Object.h>
#include <math_utils.h>

#define bbtk Toolkit::inst()
#define bbtk_ctxt bbtk.get_urho_toolkit_context()
#define LAYER_ABOVE_TEXT "Above"
#define LAYER_BELOW_TEXT "Below"
#define LAYER_ALL_TEXT "All Except"
#define LAYER_ONLY_TEXT "Only"

// Main Window
#define MAINWIN_MAPVIEWTITLE_DOCK "Map View"
#define MAINWIN_OUTVIEWTITLE_DOCK "Output View"

#define mbox(title, msg) message_box(this, title, msg, QMessageBox::Ok, QMessageBox::Warning)
#define mboxb(title, msg, b) message_box(this, title, msg, b, QMessageBox::Warning)

#define mboxq(title, msg) message_box(this, title, msg, QMessageBox::Ok, QMessageBox::Question)
#define mboxqb(title, msg, b) message_box(this, title, msg, b, QMessageBox::Question)

#define mboxc(title, msg) message_box(this, title, msg, QMessageBox::Ok, QMessageBox::Critical)
#define mboxcb(title, msg, b) message_box(this, title, msg, b, QMessageBox::Critical)

class Scene_View;

namespace Urho3D
{
class Context;
}

namespace Ui
{
class Toolkit;
}

class Toolkit : public QMainWindow
{
    Q_OBJECT

  public:
    Toolkit(QWidget * parent = 0);

    ~Toolkit();

    void init();

    bool is_init();

    static Toolkit & inst();

    Urho3D::Context * get_urho_toolkit_context();

    Ui::Toolkit * ui;

  public slots:

    void on_actionSave_View_triggered();

    void on_actionLoad_View_triggered();

    void dock_widget_floating_changed(bool floating);

    void dock_widget_area_changed(Qt::DockWidgetArea area);

  signals:

    void urho_init_complete();

  protected:
  private:

    bool init_;

    void remove_dock_widgets();

    static Toolkit * toolkit_ptr_;

    Urho3D::Context * toolkit_context_;

    Urho3D::Context * game_context_;
};