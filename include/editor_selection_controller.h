#pragma once

#include <math_utils.h>
#include <Urho3D/Graphics/OctreeQuery.h>
#include <Urho3D/Graphics/OcclusionBuffer.h>
#include <Urho3D/Container/HashSet.h>
#include <Urho3D/Scene/Component.h>

const Urho3D::String SEL_OBJ_NAME = "SelectObject";
const Urho3D::String DRAG_SELECTED_OBJECT = "DragSelectedObject";
const Urho3D::String EXTEND_SEL_OBJ_NAME = "ExtendObjectSelection";
const Urho3D::String DRAGGING = "Dragging";
const Urho3D::String ENABLE_SELECTION_RECT = "EnableSelectionRect";
const Urho3D::String Z_MOVE_HELD = "ZMoveHeld";
const Urho3D::String X_MOVE_HELD = "XMoveHeld";
const Urho3D::String Y_MOVE_HELD = "YMoveHeld";
const Urho3D::String TOGGLE_OCC_DEBUG = "ToggleOccDebug";
const Urho3D::Color SEL_RECT_BORDER_COL = Urho3D::Color(0.0f, 0.0f, 0.7f, 0.6f);
const int BORDER_SIZE = 1;
const Urho3D::Color SEL_RECT_COL = Urho3D::Color(0.0f, 0.0f, 0.7f, 0.2f);
const int Z_MOVE_FLAG = 1;
const int X_MOVE_FLAG = 2;
const int Y_MOVE_FLAG = 4;

namespace Urho3D
{
class Node;
class Camera;
class Scene;
class UIElement;
class BorderImage;
} // namespace Urho3D
class Editor_Selector;
struct Input_Context;

class Editor_Selection_Controller : public Urho3D::Component
{
    URHO3D_OBJECT(Editor_Selection_Controller, Urho3D::Component)

  public:
    Editor_Selection_Controller(Urho3D::Context * context);
    ~Editor_Selection_Controller();

    void clear_selection();

    bool is_selected(Urho3D::Node * obj_node, Urho3D::Node * sub_obj_node = nullptr);

    void snap_selection();

    void add_viewport(int vp_ind);

    void remove_viewport(int vp_ind);

    void translate_selection(const fvec3 & translation);

    void toggle_occ_debug_selection();

    void remove_from_selection(Urho3D::Node * node);

    void set_camera(Urho3D::Camera * cam);

    Urho3D::Camera * get_camera();

    void DrawDebugGeometry(bool depth);

    void handle_update(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);
    void handle_input_event(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);
    void handle_component_added(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);
    void handle_component_removed(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);

    void setup_input_context(Input_Context * ctxt);

    static void register_context(Urho3D::Context * ctxt);

  protected:
    void OnSceneSet(Urho3D::Scene * scene) override;

  private:
    void _add_to_selection_from_rect();

    HashMap<Urho3D::Node *, Vector<Urho3D::Node *>> selection_;

    HashMap<Urho3D::Node *, Vector<Urho3D::Node *>> sel_rect_selection_;

    Vector<StringHash> hashes_;

    Urho3D::Camera * cam_comp_;

    Urho3D::Scene * scene_;

    Urho3D::Vector<int> viewports_;

    Urho3D::UIElement * ui_root_;

    Urho3D::UIElement * ui_selection_rect_;

    HashMap<Urho3D::Node *, bool> cached_raycasts_;

    Urho3D::HashSet<Editor_Selector *> scene_sel_comps_;

    fvec3 frame_translation_;

    fvec4 drag_point_;

    fvec4 selection_rect_;

    fvec3 total_drag_translation_;

    bool move_allowed_;

    bool do_snap_;

    int movement_flag_;
};
