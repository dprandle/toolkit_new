#pragma once

#include <math_utils.h>
#include <Urho3D/Core/Object.h>

const float DEFAULT_MOVEMENT_SENSITIVITY = 15.0f;
const float DEFAULT_PITCH_YAW_SENSITIVITY = 100.0f;
const float DEFAULT_ZOOM_FORCE = 5000.0f;
const float DEFAULT_MOVE_FORCE = 1400.0f;
const float DEFAULT_CAMERA_MASS = 5.0f;
const float DEFAULT_DRAG_COEF = 5.0f;
const float FIXED_TIMESTEP = 0.01f;
const float MAX_MASS = 100.0f;
const float MAX_FORCE = 10000.0f;
const float MAX_ZOOM_FORCE = 20000.0f;
const float MAX_DRAG = 50.0f;
const float MAX_MOVE_SENSE = 50.0f;
const float MAX_TURN_SENSE = 200.0f;

namespace Urho3D
{
class Node;
class Camera;
}

enum Editor_Cam_Movement
{
    ECM_FORWARD = 1,
	ECM_BACK = 2,
	ECM_LEFT = 4,
	ECM_RIGHT = 8
};

struct Input_Context;

struct Force_Timer
{
	Force_Timer():
		force_(),
		time_left_(0.0f)
	{}
	
	Urho3D::Vector3 force_;
	float time_left_;
};

struct Camera_Params
{
	Camera_Params():
		movement_sensitivity_(DEFAULT_MOVEMENT_SENSITIVITY),
		pitch_yaw_sensitivity_(DEFAULT_PITCH_YAW_SENSITIVITY),
		camera_mass_(DEFAULT_CAMERA_MASS),
		drag_coef_(DEFAULT_DRAG_COEF),
		zoom_force_(DEFAULT_ZOOM_FORCE),
		move_force_(DEFAULT_MOVE_FORCE),
		invert_cam_center_(1,1),
		invert_obj_center_(1,1),
		invert_panning_(1,1)
	{}

	float movement_sensitivity_; // When free moving with mouse
	float pitch_yaw_sensitivity_;
	float camera_mass_;
	float drag_coef_;
    float zoom_force_;
	float move_force_;
	Urho3D::IntVector2 invert_cam_center_;
	Urho3D::IntVector2 invert_obj_center_;
	Urho3D::IntVector2 invert_panning_;
};

class Editor_Camera_Controller : public Urho3D::Object
{
    URHO3D_OBJECT(Editor_Camera_Controller, Urho3D::Object)

  public:

	Editor_Camera_Controller(Urho3D::Context * context);
	~Editor_Camera_Controller();

    void init();

    void release();

	void handle_update(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
	
	void handle_input_event(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);

    void set_camera(Urho3D::Camera * cam);

    void set_orbit_point(const fvec3 & point);

    fvec3 get_orbit_point() const;

    Urho3D::Camera * get_camera();

	void setup_input_context(Input_Context * ctxt);

	Camera_Params * get_camera_params();
	
  private:

	// These are instant move commands
	void _on_cam_pitch_yaw(const Urho3D::Vector2 & mdelta);

	void _on_cam_orbit(const Urho3D::Vector2 & mdelta);

    void _on_cam_orbit_set_point(const Urho3D::Vector2 & norm_mpos);

	void _on_cam_move(const Urho3D::Vector2 & mdelta);

	void _on_cam_zoom(int scroll);
	
	Urho3D::HashMap<Urho3D::StringHash, Force_Timer> forces_;

	Urho3D::Vector3 current_vel_;

	Camera_Params cp_;

	Urho3D::Camera * cam_comp_;

    Urho3D::Node * orbit_node_;
};