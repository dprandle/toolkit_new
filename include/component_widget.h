#pragma once

#include <QWidget>

namespace Urho3D{
    class String;
    class Variant;
    class Node;
    class Component;
}

class QTreeWidgetItem;
class QTreeWidget;

void build_tree_widget(Urho3D::Node * node, QTreeWidget * tw);

void create_widget_item(Urho3D::Component * comp, QTreeWidget * tw, QTreeWidgetItem * parent, const Urho3D::String & name, const Urho3D::String & value);
void create_widget_item(Urho3D::Component * comp, QTreeWidget * tw, QTreeWidgetItem * parent, const Urho3D::String & name, int value);
void create_widget_item(Urho3D::Component * comp, QTreeWidget * tw, QTreeWidgetItem * parent, const Urho3D::String & name, double value);
void create_widget_item(Urho3D::Component * comp, QTreeWidget * tw, QTreeWidgetItem * parent, const Urho3D::String & name, float value);
void create_widget_item(Urho3D::Component * comp, QTreeWidget * tw, QTreeWidgetItem * parent, const Urho3D::String & name, bool value);

class Component_Widget : public QWidget
{
    Q_OBJECT

  public:
    Component_Widget(QWidget * parent = nullptr);
    ~Component_Widget();

    void setup_ui(Urho3D::Node * node);

  private:
};