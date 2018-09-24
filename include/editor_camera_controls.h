#ifndef EDITOR_CAMERA_CONTROLS_H
#define EDITOR_CAMERA_CONTROLS_H

#include <Urho3D/Core/Object.h>

static const float default_movement_sensitivity = 15.0f;
static const float default_pitch_yaw_sensitivity = 100.0f;
static const float default_zoom_force = 2000.0f;
static const float default_move_force = 2000.0f;
static const float default_camera_mass = 15.0f;
static const float default_drag_coef = 5.0f;

#define MAX_MASS 100.0f
#define MAX_FORCE 10000.0f
#define MAX_ZOOM_FORCE 20000.0f
#define MAX_DRAG 50.0f
#define MAX_MOVE_SENSE 50.0f
#define MAX_TURN_SENSE 200.0f

namespace Urho3D
{
class Node;
class Camera;
}

enum editor_cam_movement
{
    ecm_forward = 1,
	ecm_back = 2,
	ecm_left = 4,
	ecm_right = 8
};

struct input_context;

struct force_timer
{
	force_timer():
		force(),
		time_left(0.0f)
	{}
	
	Urho3D::Vector3 force;
	float time_left;
};

struct camera_params
{
	camera_params():
		movement_sensitivity(default_movement_sensitivity),
		pitch_yaw_sensitivity(default_pitch_yaw_sensitivity),
		camera_mass(default_camera_mass),
		drag_coef(default_drag_coef),
		zoom_force(default_zoom_force),
		move_force(default_move_force),
		invert_cam_center(1,1),
		invert_obj_center(1,1),
		invert_panning(1,1)
	{}

	float movement_sensitivity; // When free moving with mouse
	float pitch_yaw_sensitivity;
	float camera_mass;
	float drag_coef;
    float zoom_force;
	float move_force;
	Urho3D::IntVector2 invert_cam_center;
	Urho3D::IntVector2 invert_obj_center;
	Urho3D::IntVector2 invert_panning;
};

class EditorCameraController : public Urho3D::Object
{
    URHO3D_OBJECT(EditorCameraController, Urho3D::Object)
  public:
	EditorCameraController(Urho3D::Context * context, Urho3D::Node * camera);
	~EditorCameraController();

	void update(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	
	void handle_input_event(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	void setup_input_context(input_context * ctxt);

	camera_params * get_camera_params();
	
  private:

	// These are instant move commands
	void _on_cam_pitch_yaw(const Urho3D::Vector2 & mdelta);
	void _on_cam_move(const Urho3D::Vector2 & mdelta);
	void _on_cam_zoom(int scroll);

	
	Urho3D::HashMap<Urho3D::StringHash, force_timer> forces;
	Urho3D::Vector3 current_vel;

	camera_params cp;
	Urho3D::Node * cam_node;
	Urho3D::Camera * cam_comp;

};
#endif
