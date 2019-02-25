#pragma once

#include <urho_window.h>

namespace Urho3D
{
class Node;
class Scene;
} // namespace Urho3D

class Editor_Selection_Controller;
class Editor_Camera_Controller;
struct Input_Context;

const float TOUCH_SENSITIVITY = 2.0f;

struct Urho_Map_Editor : public Urho_Window
{
    URHO3D_OBJECT(Urho_Map_Editor, Urho_Window)

    Urho_Map_Editor(Urho3D::Context * context, void * window_id);
    ~Urho_Map_Editor();

    void handle_input_event(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);

    void handle_scene_update(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);

    void handle_post_render_update(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);

    void handle_log_message(StringHash event_type, Urho3D::VariantMap & event_data);

    void setup_global_keys(Input_Context * ctxt);

    void create_visuals();

    Urho3D::Scene * scene_;

    Urho3D::Node * cam_node_;

    Editor_Camera_Controller * camera_controller_;

    bool draw_debug_;
};