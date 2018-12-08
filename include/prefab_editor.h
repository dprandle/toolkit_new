#pragma once

#include <urho_editor.h>

namespace Urho3D
{
class Node;
class Scene;
} // namespace Urho3D

class Editor_Selection_Controller;
class Editor_Camera_Controller;
struct Input_Context;

const float TOUCH_SENSITIVITY = 2.0f;

struct Prefab_Editor : public Urho_Editor
{
    URHO3D_OBJECT(Prefab_Editor, Urho_Editor)

    Prefab_Editor(Urho3D::Context * context, void * window_id);
    ~Prefab_Editor();

    void handle_input_event(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);

    void handle_scene_update(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);

    void handle_post_render_update(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);

    void setup_global_keys(Input_Context * ctxt);

    void create_visuals();

    Urho3D::Scene * scene_;

    Urho3D::Node * cam_node_;

    Urho3D::Node * dir_light_node_;

    Editor_Camera_Controller * camera_controller_;

    bool draw_debug_;
};