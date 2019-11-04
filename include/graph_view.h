#pragma once

#include <QMainWindow>
#include <Urho3D/Core/Object.h>

const QString GRAPH_TW_STYLE =
    R"(
QWidget
{
    background-color: rgb(30,30,30);
}

QTreeView::item
{
    background-color: rgb(30,30,30);
    padding: 5px;
    height: 15px;
}

QTreeView::branch:selected
{
  background-color: rgb(60,60,60);
}


QTreeView::item:has-children
{
    border-bottom: 1px solid rgba(200, 100, 0, 100);
}

QTreeView::item:selected
{
    background-color: rgb(60,60,60);
}

QTreeView::branch:has-children
{
    border-bottom: 1px solid rgba(200, 100, 0, 100);
}
)";

class QTreeWidget;
class QTreeWidgetItem;
class Urho_Event_Receiver;

namespace Ui
{
class Graph_View;
}

namespace Urho3D
{
class Node;
}


class Graph_View : public QMainWindow
{
    Q_OBJECT

  public:
    explicit Graph_View(QWidget * parent = 0);
    ~Graph_View();

    void urho_init_complete();

    void set_base_node(Urho3D::Node * node);

    void rebuild();

    void create_empty(Urho3D::Node * parent);

    void create_tile(Urho3D::Node * parent);

    void create_static_obj(Urho3D::Node * parent);

    void create_light(Urho3D::Node * parent);

    void create_camera(Urho3D::Node * parent);

    /// This is called when the selection system emits a selection changed event - which will occur anytime
    /// the selection changes in the main editor view
    void handle_selection_change(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);

    void handle_node_added(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);

    void handle_node_removed(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);

    void handle_node_renamed(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);

    void handle_node_cloned(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);

    public slots:
    
    void selection_changed();

  private:
    void build_recursively(QTreeWidgetItem * parent, Urho3D::Node * node);

    Urho3D::Node * base_node_;
    Ui::Graph_View * ui;
    QTreeWidget * tw_;
    Urho_Event_Receiver * event_rx;
    
};