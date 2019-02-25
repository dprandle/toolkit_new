#pragma once

#include <Urho3D/Scene/Component.h>

namespace Urho3D
{
class Node;
class Context;
class StaticModel;
} // namespace Urho3D

//using namespace Urho3D;

class Editor_Selector : public Urho3D::Component
{
    URHO3D_OBJECT(Editor_Selector, Component);

  public:
    /// Construct.
    Editor_Selector(Urho3D::Context * context);

    void set_selected(Urho3D::Node * node, bool select);

    bool is_selected(Urho3D::Node * node);

    void toggle_selected(Urho3D::Node * node);

    void set_selection_material(const Urho3D::String & name);

    void set_render_component_to_control(int comp_id);

    const Urho3D::String & selection_material();

    static void register_context(Urho3D::Context * context);

  protected:
    void OnNodeSet(Urho3D::Node * node) override;

  protected:
    //void OnNodeSet(Node * node) override;

  private:
    int id_of_component_to_control_;
    Urho3D::StaticModel * sel_render_comp_;

    Urho3D::String selected_mat_;
};