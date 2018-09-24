#pragma once

#include <QWidget>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Math/Vector3.h>

using namespace Urho3D;

namespace Urho3D
{
class Node;
class Scene;
class Sprite;
} // namespace Urho3D

class Editor_Selection_Controller;
class Editor_Camera_Controller;
class Input_Translator;
class Input_Map;
struct Input_Context;

const float TOUCH_SENSITIVITY = 2.0f;

class SceneView : public QWidget, public Urho3D::Object
{
    Q_OBJECT
    URHO3D_OBJECT(SceneView, Urho3D::Object)

    friend class Toolkit;

  public:
    SceneView(Urho3D::Context * context, QWidget * parent = nullptr);

    ~SceneView();

    void init_mouse_mode(MouseMode mode);

    void enterEvent(QEvent * e) override;

    void resizeEvent(QResizeEvent * e) override;

    void mousePressEvent(QMouseEvent * e) override;

    void mouseReleaseEvent(QMouseEvent * e) override;

    void mouseMoveEvent(QMouseEvent * e) override;

    void wheelEvent(QWheelEvent * e) override;

    void keyPressEvent(QKeyEvent * e) override;

    void keyReleaseEvent(QKeyEvent * e) override;

    // void handle_update(Urho3D::StringHash eventType, Urho3D::VariantMap & eventData);
    // void handle_render_update(Urho3D::StringHash eventType, Urho3D::VariantMap & eventData);
    // void handle_input_event(Urho3D::StringHash eventType, Urho3D::VariantMap & eventData);

  public slots:

    void run();

  private:
    void handle_input_event(StringHash event_type, VariantMap & event_data);

    void handle_scene_update(StringHash event_type, VariantMap & event_data);

    void handle_post_render_update(StringHash event_type, VariantMap & event_data);

    bool init();

    void release();

    void setup_global_keys(Input_Context * ctxt);

    void create_visuals();

    Engine * engine_;

    Scene * scene_;

    Node * cam_node_;

    Input_Translator * input_translator_;

    Editor_Camera_Controller * camera_controller_;

    Input_Map * input_map_;

    bool draw_debug_;
};