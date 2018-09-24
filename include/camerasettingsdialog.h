#ifndef CAMERASETTINGS_H
#define CAMERASETTINGS_H
#include <ui_camerasettings.h>

struct camera_params;

class CameraSettingsDialog : public QDialog
{
	Q_OBJECT
public:
	CameraSettingsDialog(QWidget * pParent=NULL);
	~CameraSettingsDialog();

	void set_camera_params(camera_params * cp_);

public slots:
	void update_params();

private:
	bool init;
	camera_params * cp;
	Ui_CameraSettingsDialog mUI;
};

#endif
