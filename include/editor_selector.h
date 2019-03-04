#pragma once

#include <Urho3D/Graphics/StaticModel.h>

namespace Urho3D
{
class Node;
class Context;
class StaticModel;
} // namespace Urho3D

//using namespace Urho3D;

class Editor_Selector : public Urho3D::StaticModel
{
    URHO3D_OBJECT(Editor_Selector, Component);

  public:
    /// Construct.
    Editor_Selector(Urho3D::Context * context);

    void set_selected(bool select);

    bool is_selected();

    void toggle_selected();

    static void register_context(Urho3D::Context * context);

  protected:
    //void OnNodeSet(Node * node) override;

  private:
};