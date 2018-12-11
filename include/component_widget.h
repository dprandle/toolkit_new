#pragma once

#include <QWidget>
#include <Urho3D/Core/Variant.h>

namespace Urho3D
{
class String;
class Node;
class Serializable;
} // namespace Urho3D

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

void create_tw_item(Urho3D::Serializable * ser,
                    const Urho3D::String & attrib_name,
                    const Urho3D::String & nested_name,
                    Urho3D::VariantVector & nested_attib_names,
                    const Urho3D::Variant & value,
                    QTreeWidget * tw,
                    QTreeWidgetItem * parent);

QWidget * create_widget_item(Urho3D::Serializable * serz,
                             Urho3D::String attrib_name,
                             Urho3D::VariantVector nested_attrib_names,
                             const Urho3D::String & value);

QWidget * create_widget_item(Urho3D::Serializable * serz,
                             Urho3D::String attrib_name,
                             Urho3D::VariantVector nested_attrib_names,
                             int value);

QWidget * create_widget_item(Urho3D::Serializable * serz,
                             Urho3D::String attrib_name,
                             Urho3D::VariantVector nested_attrib_names,
                             double value);

QWidget * create_widget_item(Urho3D::Serializable * serz,
                             Urho3D::String attrib_name,
                             Urho3D::VariantVector nested_attrib_names,
                             float value);

QWidget * create_widget_item(Urho3D::Serializable * serz,
                             Urho3D::String attrib_name,
                             Urho3D::VariantVector nested_attrib_names,
                             bool value);

class Component_Widget : public QWidget
{
    Q_OBJECT

  public:
    Component_Widget(QWidget * parent = nullptr);
    ~Component_Widget();

    void setup_ui(Urho3D::Node * node);

  private:
};