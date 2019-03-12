#pragma once

#include <QWidget>
#include <Urho3D/Core/Variant.h>
#include <QMap>
#include <QSet>

const int SB_MIN_WIDTH = 40;
const int SB_FIXED_WIDTH = 70;
namespace Urho3D
{
class String;
class Node;
class Serializable;
class AttributeInfo;
} // namespace Urho3D

const QString TW_STYLE =
    R"(
QWidget
{
    background-color: rgb(30,30,30);
    padding: 0px;
    margin: 0px;
}

QTreeView::item
{
    background-color: rgb(30,30,30);
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

void Slot_Callback(const Urho3D::Vector<Urho3D::Serializable *> & serz,
                   Urho3D::VariantVector nested_attrib_names,
                   const Urho3D::String & attrib_name,
                   const Urho3D::Variant & val);

Urho3D::Variant * Get_Attrib_Variant(Urho3D::Variant & attrib_value,
                                     Urho3D::VariantVector & nested_attrib_names);

struct cb_desc
{
    cb_desc(const Urho3D::String att_name=Urho3D::String(), const Urho3D::VariantVector & nest_names=Urho3D::Variant::emptyVariantVector, const Urho3D::Vector<Urho3D::Serializable*> & serializables = Urho3D::Vector<Urho3D::Serializable*>()):
        attrib_name(att_name),
        nested_names(nest_names),
        serz(serializables)
    {}

    std::function<void(const Urho3D::Variant &)> set_widget_value;
    Urho3D::String attrib_name;
    Urho3D::VariantVector nested_names;
    Urho3D::Vector<Urho3D::Serializable*> serz;
};

struct Create_Widget_Params
{
    Create_Widget_Params( const Urho3D::Vector<Urho3D::Serializable *> & serz,
                        const Urho3D::String & attrib_name,
                        const Urho3D::VariantVector & nested_attrib_names,
                        const Urho3D::Vector<Urho3D::Variant> & values,
                        const Urho3D::AttributeInfo & att_inf
    ):
    serz_(serz),
    attrib_name_(attrib_name),
    nested_attrib_names_(nested_attrib_names),
    values_(values),
    att_inf_(att_inf)
    {}

    Urho3D::Vector<Urho3D::Serializable *> serz_;
    Urho3D::String attrib_name_;
    Urho3D::VariantVector nested_attrib_names_;
    const Urho3D::Vector<Urho3D::Variant> & values_;
    const Urho3D::AttributeInfo & att_inf_;
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

    void do_set_widget(cb_desc * fd, const Urho3D::Vector<Urho3D::Variant> & values);

    void add_node_to_treewidget(const Urho3D::Vector<Urho3D::Node *> nodes);

    void recursive_tree_widget_check(QTreeWidgetItem * item, bool restoring);

    void create_tw_item(const Urho3D::Vector<Urho3D::Serializable *> & ser,
                        const Urho3D::String & attrib_name,
                        const Urho3D::String & nested_name,
                        Urho3D::VariantVector & nested_attib_names,
                        const Urho3D::Vector<Urho3D::Variant> & values,
                        QTreeWidgetItem * parent,
                        const Urho3D::AttributeInfo & att_inf);

    QWidget * create_string_widget_item(Create_Widget_Params params);

    QWidget * create_int_widget_item(Create_Widget_Params params);

    QWidget * create_float_widget_item(Create_Widget_Params params);

    QWidget * create_double_widget_item(Create_Widget_Params params);

    QWidget * create_bool_widget_item(Create_Widget_Params params);

    QWidget * create_vec2_widget_item(Create_Widget_Params params);

    QWidget * create_ivec2_widget_item(Create_Widget_Params params);

    QWidget * create_vec3_widget_item(Create_Widget_Params params);


    QWidget * create_ivec3_widget_item(Create_Widget_Params params);

    QWidget * create_vec4_widget_item(Create_Widget_Params params);


    QWidget * create_irect_widget_item(Create_Widget_Params params);

    QTreeWidget * tw_;
    Urho3D::Vector<Urho3D::Node*> selection_;
    QMap<QWidget*, cb_desc> updaters;
    QSet<QString> prev_expanded_items;
};