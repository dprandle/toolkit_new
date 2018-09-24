#include <editor_camera_controls.h>
#include <camerasettingsdialog.h>

CameraSettingsDialog::CameraSettingsDialog(QWidget * pParent) :
	QDialog(pParent),
	init(false)
{
    mUI.setupUi(this);
}

CameraSettingsDialog::~CameraSettingsDialog()
{}

void CameraSettingsDialog::set_camera_params(camera_params * cp_)
{
	cp = cp_;
	mUI.cmb_center_invert_x->setCurrentIndex((cp->invert_cam_center.x_ - 1) / -2);
	mUI.cmb_center_invert_y->setCurrentIndex((cp->invert_cam_center.y_ - 1) / -2);
	mUI.cmb_object_invert_x->setCurrentIndex((cp->invert_obj_center.x_ - 1) / -2);
	mUI.cmb_object_invert_y->setCurrentIndex((cp->invert_obj_center.y_ - 1) / -2);
	mUI.cmb_pan_invert_x->setCurrentIndex((cp->invert_panning.x_ - 1) / -2);
	mUI.cmb_pan_invert_y->setCurrentIndex((cp->invert_panning.y_ - 1) / -2);
	mUI.sldr_thrust->setValue(int(cp->move_force / MAX_FORCE * 100.0f));
	mUI.sldr_thrust->setValue(int(cp->zoom_force / MAX_ZOOM_FORCE * 100.0f));
	mUI.sldr_drag->setValue(int(cp->drag_coef / MAX_DRAG * 100.0f));
	mUI.sldr_mass->setValue(int(cp->camera_mass / MAX_MASS * 100.0f));
	mUI.sldr_pan->setValue(int(cp->movement_sensitivity / MAX_MOVE_SENSE * 100.0f));
	mUI.sldr_turn->setValue(int(cp->pitch_yaw_sensitivity / MAX_TURN_SENSE * 100.0f));
	init = true;
}


void CameraSettingsDialog::update_params()
{
	if (!init)
		return;
	cp->invert_cam_center.x_ = mUI.cmb_center_invert_x->currentIndex()*-2 + 1;
	cp->invert_cam_center.y_ = mUI.cmb_center_invert_y->currentIndex()*-2 + 1;
	cp->invert_obj_center.x_ = mUI.cmb_object_invert_x->currentIndex()*-2 + 1;
	cp->invert_obj_center.y_ = mUI.cmb_object_invert_y->currentIndex()*-2 + 1;
	cp->invert_panning.x_ = mUI.cmb_pan_invert_x->currentIndex()*-2 + 1;
	cp->invert_panning.y_ = mUI.cmb_pan_invert_y->currentIndex()*-2 + 1;
	cp->move_force = float(mUI.sldr_thrust->value()) / 100.0f * MAX_FORCE;
	cp->zoom_force = float(mUI.sldr_thrust->value()) / 100.0f * MAX_ZOOM_FORCE;
	cp->drag_coef = float(mUI.sldr_drag->value()) / 100.0f * MAX_DRAG;
	cp->camera_mass = float(mUI.sldr_mass->value()) / 100.0f * MAX_MASS;
	cp->movement_sensitivity = float(mUI.sldr_pan->value()) / 100.0f * MAX_MOVE_SENSE;
	cp->pitch_yaw_sensitivity = float(mUI.sldr_turn->value()) / 100.0f * MAX_TURN_SENSE;
}
