#include <QTimer>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Urho3DAll.h>
#include <camerasettingsdialog.h>
#include <editor_camera_controls.h>
#include <editor_gizmo_component.h>
#include <editor_selection.h>
#include <input_translator.h>
#include <scene_view.h>
#include <toolkit.h>

using namespace Urho3D;

SceneView::SceneView(Context* context, QWidget* parent)
    : QWidget(parent)
    , Object(context)
    , ctxt(context)
    , eng(new Engine(ctxt))
    , m_input_map(new InputMap())
    , input_system(new InputTranslator(ctxt))
    , cam_controller(nullptr)
    , selection(new EditorSelection(ctxt))
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(SceneView, handle_update));
    SubscribeToEvent(E_RENDERUPDATE,
        URHO3D_HANDLER(SceneView, handle_render_update));
    SubscribeToEvent(E_RENDERUPDATE,
        URHO3D_HANDLER(SceneView, handle_render_update));
    SubscribeToEvent(E_INPUT_TRIGGER,
        URHO3D_HANDLER(SceneView, handle_input_event));

    setFocusPolicy(Qt::ClickFocus);
    setMouseTracking(true);
}

SceneView::~SceneView() {}

void SceneView::init()
{
    VariantMap engine_params;
    engine_params["FrameLimiter"] = false;
    engine_params["LogName"] = GetSubsystem<FileSystem>()->GetProgramDir() + "BBToolkit.log";
    engine_params["FullScreen"] = false;
    engine_params["WindowWidth"] = 1280;
    engine_params["WindowHeight"] = 720;
    engine_params["WindowResizable"] = true;
    engine_params["ExternalWindow"] = (void*)(winId());
    eng->Initialize(engine_params);

    // Get default style
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    // Create console
    Console* console = eng->CreateConsole();
    console->GetBackground()->SetOpacity(0.8f);
    console->SetDefaultStyle(xmlFile);

    // Create debug HUD.
    DebugHud* debugHud = eng->CreateDebugHud();
    debugHud->SetDefaultStyle(xmlFile);

    ctxt->RegisterFactory<EditorGizmoComponent>();

    scene = new Scene(ctxt);
    scene->CreateComponent<Urho3D::Octree>();
    scene->CreateComponent<DebugRenderer>();

    editor_cam_node = new Node(ctxt);
    Camera* editor_cam = editor_cam_node->CreateComponent<Camera>();
    editor_cam_node->SetPosition(Vector3(0, 0, 5));
    editor_cam_node->SetDirection(Vector3(0, 0, -1));
    cam_controller = new EditorCameraController(ctxt, editor_cam_node);
    bbtk.camera_settings()->set_camera_params(
        cam_controller->get_camera_params());

    Renderer* rnd = GetSubsystem<Renderer>();

    Viewport* vp = new Viewport(ctxt, scene, editor_cam);
    rnd->SetViewport(0, vp);
    RenderPath* rp = new RenderPath;
    rp->Load(cache->GetResource<XMLFile>("RenderPaths/Deferred.xml"));
    vp->SetRenderPath(rp);

    Node* skyNode = scene->CreateChild("Sky");
    skyNode->Rotate(Quaternion(90.0f, Vector3(1, 0, 0)));
    Skybox* skybox = skyNode->CreateComponent<Skybox>();
    skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox.xml"));

    // Create a directional light
    Node* light_node = scene->CreateChild("Dir_Light");
    light_node->SetDirection(Vector3(-0.0f, -0.5f, -1.0f));
    Light* light = light_node->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetColor(Color(1.0f, 1.0f, 1.0f));
    light->SetSpecularIntensity(5.0f);
    light->SetBrightness(1.0f);
    

    // Create StaticModelGroups in the scene
    StaticModelGroup* lastGroup = nullptr;
    Material* grass_tile = cache->GetResource<Material>("Materials/Tiles/Grass.xml");
    // Texture2D * diff_grass =
    // cache->GetResource<Texture2D>("Textures/Tiles/diffuseGrass.png");
    // Texture2D * norm_grass =
    // cache->GetResource<Texture2D>("Textures/Tiles/normalGrass.png");
    // grass_tile->SetTexture(Urho3D::TU_DIFFUSE, diff_grass);
    // grass_tile->SetTexture(Urho3D::TU_NORMAL, norm_grass);
    // grass_tile->SetTechnique(0,
    // cache->GetResource<Technique>("Techniques/DiffNormal.xml"));

    for (int y = -50; y < 50; ++y) {
        for (int x = -50; x < 50; ++x) {
            if (!lastGroup || lastGroup->GetNumInstanceNodes() >= 25 * 25) {
                Node* tile_group_node = scene->CreateChild("Grass_Tile_Group");
                lastGroup = tile_group_node->CreateComponent<StaticModelGroup>();
                lastGroup->SetModel(
                    cache->GetResource<Model>("Models/Tiles/Grass.mdl"));
                lastGroup->SetMaterial(grass_tile);
            }

            Node* tile_node = scene->CreateChild("Grass_Tile");
            tile_node->SetPosition(Vector3(x, y, 0));
            tile_node->SetScale(0.5f);
            tile_node->SetRotation(Quaternion(90.0f, Vector3(1.0f, 0.0f, 0.0f)));
            lastGroup->AddInstanceNode(tile_node);
        }
    }

    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_timeout()));
    timer->start();

    input_context* in_context = m_input_map->create_context("global_context");
    cam_controller->setup_input_context(in_context);
    setup_input(in_context);
    input_system->push_context(in_context);

    selection->set_scene(scene);
    selection->set_camera(editor_cam_node);
    selection->setup_input_context(in_context);
}

void SceneView::setup_input(input_context * ctxt)
{
    trigger_condition tc;
    input_action_trigger * it = nullptr;

	tc.key = KEY_ESCAPE;
	tc.mouse_button = 0;

	it = ctxt->create_trigger(tc);
 	it->name = "CloseWindow";
	it->trigger_state = t_end;
	it->mb_required = 0;
	it->mb_allowed = MOUSEB_ANY;
	it->qual_required = 0;
	it->qual_allowed = QUAL_ANY;

	tc.key = KEY_1;
	tc.mouse_button = 0;

	it = ctxt->create_trigger(tc);
 	it->name = "ToggleConsole";
	it->trigger_state = t_begin;
	it->mb_required = 0;
	it->mb_allowed = MOUSEB_ANY;
	it->qual_required = 0;
	it->qual_allowed = QUAL_ANY;

	tc.key = KEY_2;
	tc.mouse_button = 0;

	it = ctxt->create_trigger(tc);
 	it->name = "ToggleDebugHUD";
	it->trigger_state = t_begin;
	it->mb_required = 0;
	it->mb_allowed = MOUSEB_ANY;
	it->qual_required = 0;
	it->qual_allowed = QUAL_ANY;

	tc.key = KEY_F9;
	tc.mouse_button = 0;

	it = ctxt->create_trigger(tc);
 	it->name = "TakeScreenshot";
	it->trigger_state = t_begin;
	it->mb_required = 0;
	it->mb_allowed = MOUSEB_ANY;
	it->qual_required = 0;
	it->qual_allowed = QUAL_ANY;    
}

void SceneView::release()
{
    delete editor_cam_node;
    delete scene;
    delete eng;
    delete m_input_map;
    delete input_system;
    delete ctxt;
}

void SceneView::handle_update(StringHash eventType, VariantMap& eventData)
{
    //	editor_cam_node->Translate(current_trans);
    // editor_cam_node->Rotate(Quaternion(10*eventData["TimeStep"].GetFloat(),Vector3(0.0f,0.0f,1.0f)));
}

void SceneView::handle_input_event(Urho3D::StringHash eventType,
    Urho3D::VariantMap& eventData)
{
    StringHash name = eventData["trigger_name"].GetStringHash();
    int state = eventData["trigger_state"].GetInt();
    Vector2 norm_mpos = eventData["normalized_mouse_pos"].GetVector2();
    Vector2 norm_mpos_delta = eventData["normalized_mouse_delta"].GetVector2();
    int mouse_wheel = eventData["mouse_wheel"].GetInt();

    if (name == StringHash("editor_stacked!!"))
        bbtk.output_view()->writeToScreen("Editor Stacked!!");

    bbtk.output_view()->writeToScreen("Trigger name: " + QString(name.ToString().CString()));
    bbtk.output_view()->writeToScreen("Trigger state: " + QString::number(state));
    bbtk.output_view()->writeToScreen("Norm mpos: " + QString(norm_mpos.ToString().CString()));
    bbtk.output_view()->writeToScreen(
        "Norm mdelta: " + QString(norm_mpos_delta.ToString().CString()));
    bbtk.output_view()->writeToScreen("Wheel: " + QString::number(mouse_wheel) + "\n");

    if (name == StringHash("CloseWindow")) {
        Console* console = GetSubsystem<Console>();
        if (console->IsVisible())
            console->SetVisible(false);
        else
            eng->Exit();
    } else if (name == StringHash("ToggleDebugHUD")) {
        GetSubsystem<DebugHud>()->ToggleAll();
    } else if (name == StringHash("ToggleConsole")) {
        GetSubsystem<Console>()->Toggle();
    } else if (name == StringHash("TakeScreenshot")) {
        Graphics* graphics = GetSubsystem<Graphics>();
        Image screenshot(context_);
        graphics->TakeScreenShot(screenshot);
        // Here we save in the Data folder with date and time appended
        screenshot.SavePNG(
            GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Screenshot_" + Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_') + ".png");
    }
}

void SceneView::handle_render_update(StringHash eventType,
    VariantMap& eventData) {}

void SceneView::on_timeout()
{
    if (!eng->IsExiting())
        eng->RunFrame();
}

void SceneView::resizeEvent(QResizeEvent* e)
{
    input_system->qt_window_resize(e);
    QWidget::resizeEvent(e);
}

void SceneView::enterEvent(QEvent* e) { QWidget::enterEvent(e); }

void SceneView::mousePressEvent(QMouseEvent* e)
{
    input_system->qt_mouse_press(e);
    QWidget::mousePressEvent(e);
}

void SceneView::mouseReleaseEvent(QMouseEvent* e)
{
    input_system->qt_mouse_release(e);
    QWidget::mouseReleaseEvent(e);
}

void SceneView::mouseMoveEvent(QMouseEvent* e)
{
    input_system->qt_mouse_move(e);
    QWidget::mouseMoveEvent(e);
}

void SceneView::wheelEvent(QWheelEvent* e)
{
    input_system->qt_mouse_wheel(e);
    QWidget::wheelEvent(e);
}

void SceneView::keyPressEvent(QKeyEvent* e)
{
    input_system->qt_key_press(e);
    QWidget::keyPressEvent(e);
}

void SceneView::keyReleaseEvent(QKeyEvent* e)
{
    input_system->qt_key_release(e);
    QWidget::keyReleaseEvent(e);
}
