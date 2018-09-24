#pragma once

#include <ui_camerasettings.h>

struct Camera_Params;

class CameraSettingsDialog : public QDialog
{
	Q_OBJECT
public:
	CameraSettingsDialog(QWidget * pParent=NULL);
	~CameraSettingsDialog();

	void set_camera_params(Camera_Params * cp_);

public slots:
	void update_params();

private:
	bool init;
	Camera_Params * cp;
	Ui_CameraSettingsDialog mUI;
};