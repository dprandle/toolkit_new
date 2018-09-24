#ifndef SCENE_VIEW_H
#define SCENE_VIEW_H

#include <QMap>
#include <QWidget>
#include <Urho3D/Core/Object.h>

namespace Urho3D {
class Engine;
class Scene;
class Node;
}

class InputTranslator;
class InputMap;
class EditorCameraController;
class EditorSelection;
struct input_context;

class SceneView : public QWidget, public Urho3D::Object {
    Q_OBJECT
    URHO3D_OBJECT(SceneView, Urho3D::Object)

    friend class Toolkit;

public:
    SceneView(Urho3D::Context* context, QWidget* parent = NULL);
    ~SceneView();

    void init();
    void release();

    void enterEvent(QEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void keyReleaseEvent(QKeyEvent* e) override;

    void handle_update(Urho3D::StringHash eventType,
        Urho3D::VariantMap& eventData);
    void handle_render_update(Urho3D::StringHash eventType,
        Urho3D::VariantMap& eventData);
    void handle_input_event(Urho3D::StringHash eventType,
        Urho3D::VariantMap& eventData);

public slots:

    void on_timeout();

private:

    void setup_input(input_context * ctxt);
    
    Urho3D::Vector3 current_trans;
    Urho3D::Context* ctxt;
    Urho3D::Engine* eng;
    Urho3D::Scene* scene;
    Urho3D::Node* editor_cam_node;
    InputMap* m_input_map;
    InputTranslator* input_system;
    EditorCameraController* cam_controller;
    EditorSelection* selection;
};
#endif
