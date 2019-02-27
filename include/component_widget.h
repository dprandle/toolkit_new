#pragma once

#include <QWidget>
#include <Urho3D/Core/Variant.h>
#include <QMap>

namespace Urho3D
{
class String;
class Node;
class Serializable;
} // namespace Urho3D

const QString TW_STYLE =
    R"(
QWidget
{
    background-color: rgb(30,30,30);
    margin: 0px;
    padding: 0px;
}

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

QTreeView::item:has-children
{
    border-bottom: 1px solid rgba(200, 100, 0, 100);
}

QTreeView::branch:has-children
{
    border-bottom: 1px solid rgba(200, 100, 0, 100);
}
)";

class QTreeWidgetItem;
class QTreeWidget;

struct cb_desc
{
    cb_desc(const Urho3D::String att_name=Urho3D::String(), const Urho3D::VariantVector & nest_names=Urho3D::Variant::emptyVariantVector):
        attrib_name(att_name),
        nested_names(nest_names)
    {}

    std::function<void(const Urho3D::Variant &)> set_widget_value;
    Urho3D::String attrib_name;
    Urho3D::VariantVector nested_names;
};

class Component_Widget : public QWidget
{
    Q_OBJECT

  public:
    Component_Widget(QWidget * parent = nullptr);
    ~Component_Widget();

    void setup_ui(const Urho3D::Vector<Urho3D::Node *> & nodes);

    void update_tw_from_node();

  private:

    void add_node_to_treewidget(Urho3D::Node * node);

    void create_tw_item(Urho3D::Serializable * ser,
                        const Urho3D::String & attrib_name,
                        const Urho3D::String & nested_name,
                        Urho3D::VariantVector & nested_attib_names,
                        const Urho3D::Variant & value,
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

    QWidget * create_widget_item(Urho3D::Serializable * serz,
                                 Urho3D::String attrib_name,
                                 Urho3D::VariantVector nested_attrib_names,
                                 const Urho3D::Vector2 & value);

    QWidget * create_widget_item(Urho3D::Serializable * serz,
                                 Urho3D::String attrib_name,
                                 Urho3D::VariantVector nested_attrib_names,
                                 const Urho3D::IntVector2 & value);

    QWidget * create_widget_item(Urho3D::Serializable * serz,
                                 Urho3D::String attrib_name,
                                 Urho3D::VariantVector nested_attrib_names,
                                 const Urho3D::Vector3 & value);


    QWidget * create_widget_item(Urho3D::Serializable * serz,
                                 Urho3D::String attrib_name,
                                 Urho3D::VariantVector nested_attrib_names,
                                 const Urho3D::IntVector3 & value);

    QWidget * create_widget_item(Urho3D::Serializable * serz,
                                 Urho3D::String attrib_name,
                                 Urho3D::VariantVector nested_attrib_names,
                                 const Urho3D::Vector4 & value);


    QWidget * create_widget_item(Urho3D::Serializable * serz,
                                 Urho3D::String attrib_name,
                                 Urho3D::VariantVector nested_attrib_names,
                                 const Urho3D::IntRect & value);

    QTreeWidget * tw_;
    Urho3D::Vector<Urho3D::Node*> selection_;
    QMap<Urho3D::Serializable*, cb_desc> updaters;
};