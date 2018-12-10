#pragma once

#include <QWidget>

namespace Urho3D{
    class String;
    class Variant;
    class Node;
    class Serializable;
}

const QString TW_STYLE = 
R"(
QTreeView
{
    padding: 0px;
    margin: 0px;
}

QTreeView::item
{
    padding: 5px;
    height: 20px;
}

QCheckBox
{
    background-color: rgb(30,30,30);
}

QTreeView::item:has-children
{
    border-top: 1px solid rgb(200, 100, 0);
}

QTreeView::branch:has-children
{
    border-top: 1px solid rgb(200, 100, 0);
}
)";

class QTreeWidgetItem;
class QTreeWidget;

void build_tree_widget(Urho3D::Node * node, QTreeWidget * tw);
void create_tw_item(Urho3D::Serializable * ser, const Urho3D::String & var_name, QTreeWidget * tw, QTreeWidgetItem * parent);
QWidget * create_widget_item(Urho3D::Serializable * serz, const Urho3D::String & name, const Urho3D::String & value);
QWidget * create_widget_item(Urho3D::Serializable * serz, const Urho3D::String & name, int value);
QWidget * create_widget_item(Urho3D::Serializable * serz, const Urho3D::String & name, double value);
QWidget * create_widget_item(Urho3D::Serializable * serz, const Urho3D::String & name, float value);
QWidget * create_widget_item(Urho3D::Serializable * serz, const Urho3D::String & name, bool value);

class Component_Widget : public QWidget
{
    Q_OBJECT

  public:
    Component_Widget(QWidget * parent = nullptr);
    ~Component_Widget();

    void setup_ui(Urho3D::Node * node);

  private:
};