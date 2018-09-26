#include <editor_camera_controller.h>
#include <input_translator.h>
#include <mtdebug_print.h>

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/Context.h>

using namespace Urho3D;

Editor_Camera_Controller::Editor_Camera_Controller(Urho3D::Context * context)
    : Object(context), cp_(), cam_comp_(nullptr), orbit_node_(new Node(context))
{}

Editor_Camera_Controller::~Editor_Camera_Controller()
{}

void Editor_Camera_Controller::init()
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Editor_Camera_Controller, handle_update));
    SubscribeToEvent(E_INPUT_TRIGGER, URHO3D_HANDLER(Editor_Camera_Controller, handle_input_event));
}

void Editor_Camera_Controller::release()
{
    UnsubscribeFromAllEvents();
}

void Editor_Camera_Controller::set_camera(Urho3D::Camera * cam)
{
    cam_comp_ = cam;
    orbit_node_->SetRotation(fquat());
    orbit_node_->SetPosition(fvec3());
    orbit_node_->SetScale(1.0f);
    cam_comp_->GetNode()->SetParent(orbit_node_);
}

Urho3D::Camera * Editor_Camera_Controller::get_camera()
{
    return cam_comp_;
}

Camera_Params * Editor_Camera_Controller::get_camera_params()
{
    return &cp_;
}

void Editor_Camera_Controller::add_viewport(int vp_ind)
{
    if (!viewports_.Contains(vp_ind))
        viewports_.Push(vp_ind);
}

void Editor_Camera_Controller::remove_viewport(int vp_ind)
{
    viewports_.Remove(vp_ind);
}

void Editor_Camera_Controller::handle_update(Urho3D::StringHash event_type,
                                             Urho3D::VariantMap & event_data)
{
    if (cam_comp_ == nullptr)
        return;

    Urho3D::Node * cam_node = cam_comp_->GetNode();

    float dt = event_data["TimeStep"].GetFloat();
    static double accumulator = 0.0f;
    accumulator += dt;

    while (accumulator >= FIXED_TIMESTEP)
    {
        Vector3 resultant;
        auto iter = forces_.Begin();
        while (iter != forces_.End())
        {
            Force_Timer & ft = iter->second_;

            if (iter->first_ == StringHash("CameraForward"))
            {
                float mag = ft.force_.Length();
                ft.force_ = cam_node->GetDirection() * mag;
            }
            else if (iter->first_ == StringHash("CameraReverse"))
            {
                float mag = ft.force_.Length();
                ft.force_ = cam_node->GetDirection() * -mag;
            }
            else if (iter->first_ == StringHash("CameraLeft"))
            {
                float mag = ft.force_.Length();
                ft.force_ = cam_node->GetRight() * -mag;
            }
            else if (iter->first_ == StringHash("CameraRight"))
            {
                float mag = ft.force_.Length();
                ft.force_ = cam_node->GetRight() * mag;
            }

            resultant += ft.force_;
            ft.time_left_ -= FIXED_TIMESTEP;
            if (ft.time_left_ <= 0.0f && ft.time_left_ > -0.9f)
                iter = forces_.Erase(iter);
            else
                ++iter;
        }

        Vector3 drag = current_vel_.Normalized() * current_vel_.LengthSquared() * cp_.drag_coef_;
        resultant -= drag;

        Vector3 vh = current_vel_ + resultant * FIXED_TIMESTEP / (2.0f * cp_.camera_mass_);
        Vector3 new_pos = cam_node->GetPosition() + vh * FIXED_TIMESTEP;
        current_vel_ = (vh + resultant * FIXED_TIMESTEP / (2.0f * cp_.camera_mass_)) * 0.99f;

        // Clamp velocity down
        float vel_len = current_vel_.Length();
        if (vel_len <= 0.5f && vel_len >= -0.5f)
            current_vel_ = Vector3();

        cam_node->SetPosition(new_pos);

        accumulator -= FIXED_TIMESTEP;
    }
}

void Editor_Camera_Controller::setup_input_context(Input_Context * ctxt)
{
    Input_Action_Trigger it;

    it.condition_.key_ = KEY_W;
    it.condition_.mouse_button_ = 0;
    it.name_ = "CameraForward";
    it.trigger_state_ = T_BEGIN | T_END;
    it.mb_required_ = 0;
    it.mb_allowed_ = MOUSEB_ANY;
    it.qual_required_ = 0;
    it.qual_allowed_ = QUAL_ANY;
    ctxt->create_trigger(it);

    it.condition_.key_ = KEY_S;
    it.name_ = "CameraReverse";
    ctxt->create_trigger(it);

    it.condition_.key_ = KEY_A;
    it.name_ = "CameraLeft";
    ctxt->create_trigger(it);

    it.condition_.key_ = KEY_D;
    it.name_ = "CameraRight";
    ctxt->create_trigger(it);

    it.condition_.key_ = 0;
    it.condition_.mouse_button_ = MOUSEB_RIGHT;
    it.name_ = "SetOrbitPoint";
    it.trigger_state_ = T_BEGIN;
    it.mb_required_ = 0;
    it.mb_allowed_ = 0;
    it.qual_required_ = 0;
    it.qual_allowed_ = QUAL_ANY;
    ctxt->create_trigger(it);

    it.condition_.key_ = 0;
    it.condition_.mouse_button_ = MOUSEB_MOVE;
    it.name_ = "CameraPitchYaw";
    it.trigger_state_ = T_BEGIN;
    it.mb_required_ = MOUSEB_MIDDLE;
    it.mb_allowed_ = 0;
    it.qual_required_ = 0;
    it.qual_allowed_ = QUAL_ANY;
    ctxt->create_trigger(it);

    it.name_ = "OrbitCam";
    it.mb_required_ = MOUSEB_RIGHT;
    it.mb_allowed_ = 0;
    ctxt->create_trigger(it);

    it.name_ = "CameraMove";
    it.mb_required_ = MOUSEB_LEFT | MOUSEB_RIGHT;
    it.mb_allowed_ = MOUSEB_ANY;
    ctxt->create_trigger(it);

    it.condition_.mouse_button_ = MOUSEB_WHEEL;
    it.name_ = "CameraZoom";
    it.mb_required_ = 0;
    it.mb_allowed_ = MOUSEB_ANY;
    ctxt->create_trigger(it);
}

void Editor_Camera_Controller::set_orbit_point(const fvec3 & point)
{
    if (cam_comp_ == nullptr)
        return;
    Node * cam_node = cam_comp_->GetNode();

    fvec3 cur_world_pos = cam_node->GetWorldPosition();
    //fquat cur_orientation = cam_node->GetWorldRotation();
    orbit_node_->SetWorldPosition(point);
    cam_node->SetWorldPosition(cur_world_pos);
}

fvec3 Editor_Camera_Controller::get_orbit_point() const
{
    return orbit_node_->GetWorldPosition();
}

void Editor_Camera_Controller::_on_cam_pitch_yaw(const Urho3D::Vector2 & mdelta)
{
    if (cam_comp_ == nullptr)
        return;
    Urho3D::Node * cam_node = cam_comp_->GetNode();

    Quaternion pitch(mdelta.y_ * cp_.invert_cam_center_.y_ * cp_.pitch_yaw_sensitivity_,
                     cam_node->GetWorldRight());
    Quaternion yaw(mdelta.x_ * cp_.invert_cam_center_.x_ * cp_.pitch_yaw_sensitivity_,
                   Vector3(0.0f, 0.0f, 1.0f));
    cam_node->Rotate(pitch * yaw, TS_WORLD);
}

void Editor_Camera_Controller::_on_cam_orbit_set_point(const Urho3D::Vector2 & norm_mpos, int vp_ind)
{
    if (cam_comp_ == nullptr)
        return;

    Urho3D::Node * cam_node = cam_comp_->GetNode();
    Renderer * rnd = GetSubsystem<Renderer>();
    Viewport * vp = rnd->GetViewport(vp_ind);

    if (vp == nullptr)
        return;

    Scene * scene_ = vp->GetScene();
    if (scene_ == nullptr)
        return;
    
    Octree * oct = scene_->GetComponent<Octree>();
    PODVector<RayQueryResult> res;
    RayOctreeQuery q(res, cam_comp_->GetScreenRay(norm_mpos.x_, norm_mpos.y_));
    oct->RaycastSingle(q);

    fvec3 pos = cam_node->GetWorldPosition() + cam_node->GetWorldDirection().Normalized() * 10.0f;

    // If raycast not empty then move the orbit node to position
    if (!q.result_.Empty())
        pos = q.result_[0].position_;

    set_orbit_point(pos);
}

void Editor_Camera_Controller::_on_cam_orbit(const Urho3D::Vector2 & mdelta)
{
    if (cam_comp_ == nullptr)
        return;
    Urho3D::Node * cam_node = cam_comp_->GetNode();

    Quaternion pitch(mdelta.y_ * cp_.invert_cam_center_.y_ * cp_.pitch_yaw_sensitivity_,
                     cam_node->GetWorldRight());
    Quaternion yaw(mdelta.x_ * cp_.invert_cam_center_.x_ * cp_.pitch_yaw_sensitivity_,
                   Vector3(0.0f, 0.0f, 1.0f));
    //parent_node_->SetPosition()
    orbit_node_->Rotate(yaw * pitch, TS_WORLD);
}

void Editor_Camera_Controller::_on_cam_move(const Urho3D::Vector2 & mdelta)
{
    if (cam_comp_ == nullptr)
        return;
    Urho3D::Node * cam_node = cam_comp_->GetNode();

    Vector3 translation = cam_node->GetRight() * -mdelta.x_ * cp_.invert_panning_.x_ +
                          cam_node->GetUp() * mdelta.y_ * cp_.invert_panning_.y_;
    cam_node->SetPosition(cam_node->GetPosition() + translation * cp_.movement_sensitivity_);
}

void Editor_Camera_Controller::_on_cam_zoom(int wheel)
{
    Force_Timer ref;
    auto iter = forces_.Find("CameraZoom");
    if (iter != forces_.End())
        ref = iter->second_;

    ref.time_left_ = 0.01f;
    if (wheel > 0)
        ref.force_ += cam_comp_->GetNode()->GetDirection() * cp_.zoom_force_;
    else if (wheel < 0)
        ref.force_ += cam_comp_->GetNode()->GetDirection() * -cp_.zoom_force_;

    forces_["CameraZoom"] = ref;
}

void Editor_Camera_Controller::handle_input_event(Urho3D::StringHash event_type,
                                                  Urho3D::VariantMap & event_data)
{
    if (cam_comp_ == nullptr)
        return;
    Urho3D::Node * cam_node = cam_comp_->GetNode();

    StringHash name = event_data[InputTrigger::P_TRIGGER_NAME].GetStringHash();
    int state = event_data[InputTrigger::P_TRIGGER_STATE].GetInt();
    Vector2 norm_mpos = event_data[InputTrigger::P_VIEWPORT_NORM_MPOS].GetVector2();
    Vector2 norm_mdelta = event_data[InputTrigger::P_VIEWPORT_NORM_MDELTA].GetVector2();
    int wheel = event_data[InputTrigger::P_MOUSE_WHEEL].GetInt();
    int vp_ind = event_data[InputTrigger::P_VIEWPORT_INDEX].GetInt();

    if (!viewports_.Contains(vp_ind))
        return;

    Force_Timer ft;
    ft.time_left_ = 0.0f;

    if (name == StringHash("CameraForward"))
    {
        ft.time_left_ = -1.0f; // do not auto remove this force
        ft.force_ = cam_node->GetDirection() * cp_.move_force_;
        if (state == T_BEGIN)
            forces_[name] = ft;
        else
            forces_.Erase(name);
    }
    else if (name == StringHash("CameraReverse"))
    {
        ft.time_left_ = -1.0f; // do not auto remove this force
        ft.force_ = cam_node->GetDirection() * -cp_.move_force_;
        if (state == T_BEGIN)
            forces_[name] = ft;
        else
            forces_.Erase(name);
    }
    else if (name == StringHash("CameraLeft"))
    {
        ft.time_left_ = -1.0f; // do not auto remove this force
        ft.force_ = cam_node->GetRight() * -cp_.move_force_;
        if (state == T_BEGIN)
            forces_[name] = ft;
        else
            forces_.Erase(name);
    }
    else if (name == StringHash("CameraRight"))
    {
        ft.time_left_ = -1.0f; // do not auto remove this force
        ft.force_ = cam_node->GetRight() * cp_.move_force_;
        if (state == T_BEGIN)
            forces_[name] = ft;
        else
            forces_.Erase(name);
    }
    else if (name == StringHash("CameraPitchYaw"))
    {
        _on_cam_pitch_yaw(norm_mdelta);
    }
    else if (name == StringHash("SetOrbitPoint"))
    {
        _on_cam_orbit_set_point(norm_mpos, vp_ind);
    }
    else if (name == StringHash("OrbitCam"))
    {
        _on_cam_orbit(norm_mdelta);
    }
    else if (name == StringHash("CameraMove"))
    {
        _on_cam_move(norm_mdelta);
    }
    else if (name == StringHash("CameraZoom"))
    {
        _on_cam_zoom(wheel);
    }
}