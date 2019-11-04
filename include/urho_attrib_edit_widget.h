#pragma once

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Core/Attribute.h>
#include <QWidget>
#include <functional>

struct Selected_Attrib_Desc
{
    Selected_Attrib_Desc(
        const Urho3D::String att_name = Urho3D::String(),
        const Urho3D::VariantVector & nest_names = Urho3D::Variant::emptyVariantVector,
        const Urho3D::Vector<Urho3D::Serializable *> & serializables =
            Urho3D::Vector<Urho3D::Serializable *>(),
        const Urho3D::VariantVector & attrib_values_ = Urho3D::Variant::emptyVariantVector)
        : attrib_name(att_name),
          nested_names(nest_names),
          serz(serializables),
          attrib_values(attrib_values_)
    {}

    std::function<void(const Urho3D::Variant &)> set_widget_value;

    /// Base attribute name - ie the actual Urho attribute
    Urho3D::String attrib_name;

    /// If the base attribute is a map or vector, this is a vector of string or int keys
    /// if a string key then the attrib is variant map, if int key then variant vector. The
    /// number of keys indicates the level of nested attribute - IE if first key is "la" and
    /// second key is (int) 4, then this is the 4th item in a variant vector, which is the value
    /// associated with the key "la" in the variant map attribute names "attrib_name"
    Urho3D::VariantVector nested_names;

    // This is the selected serializable - would be nodes but we are attempting to reuse this
    // inspector for other serializables, such as resources too
    Urho3D::Vector<Urho3D::Serializable *> serz;

    // This is the value of the narrowed attribute.. it corresponds with selected serializables just
    // like the above - IE if only one item is selected this vector and the vector above would have
    // a single item
    Urho3D::VariantVector attrib_values;

    // Information and getter/setter for the attribute
    Urho3D::AttributeInfo attrib_info;
};

Q_DECLARE_METATYPE(Selected_Attrib_Desc)

struct Urho_Attrib_Edit_Widget_Ui
{
    virtual void setup_ui(QWidget * widget) = 0;
};

namespace Ui{
    struct Vector2D_Int_Editor;
}

struct Urho_Vec2D_Int_Ui : public Urho_Attrib_Edit_Widget_Ui
{
    Urho_Vec2D_Int_Ui();
    ~Urho_Vec2D_Int_Ui();

    void setup_ui(QWidget * widget);
    
    Ui::Vector2D_Int_Editor * ui;
};

class Attrib_Edit_Widget : public QWidget
{
  public:
    Attrib_Edit_Widget(Urho3D::VariantType type, QWidget * parent = nullptr);
    ~Attrib_Edit_Widget();
private:
    void create_ui(Urho3D::VariantType type);
    Urho_Attrib_Edit_Widget_Ui * ui;
};