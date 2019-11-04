#include <urho_common.h>

#include <urho_map_editor.h>
#include <mtdebug_print.h>
#include <tile_occupier.h>
#include <hex_tile_grid.h>
#include <editor_selector.h>
#include <editor_selection_controller.h>
#include <editor_camera_controller.h>
#include <input_translator.h>
#include <toolkit.h>
#include <console.h>
#include <ui_toolkit.h>

Urho_Map_Editor::Urho_Map_Editor(Context * context, void * window_id)
    : Urho_Window(context, window_id),
      scene_(nullptr),
      cam_node_(nullptr),
      camera_controller_(new Editor_Camera_Controller(context)),
      draw_debug_(false)
{
    VariantMap engine_params;
    engine_params[EP_FRAME_LIMITER] = false;
    engine_params[EP_LOG_NAME] = GetSubsystem<FileSystem>()->GetProgramDir() + "BBToolkit.log";
    engine_params[EP_FULL_SCREEN] = false;
    engine_params[EP_WINDOW_WIDTH] = 1280;
    engine_params[EP_WINDOW_HEIGHT] = 720;
    engine_params[EP_WINDOW_RESIZABLE] = true;
    engine_params[EP_EXTERNAL_WINDOW] = window_id;

    // Register custom systems
    context_->RegisterSubsystem(camera_controller_);

    // Scene root components
    Editor_Selection_Controller::register_context(context_);
    Hex_Tile_Grid::register_context(context_);

    // Per node components
    Tile_Occupier::register_context(context_);
    Editor_Selector::register_context(context_);

    if (!engine_->Initialize(engine_params))
    {
        // Do some logging
        return;
    }

    context_->RegisterSubsystem(new Script(context_));
    context_->RegisterSubsystem(new LuaScript(context_));

    input_translator_->init();
    camera_controller_->init();

    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(Urho_Map_Editor, handle_scene_update));
    SubscribeToEvent(E_INPUT_TRIGGER, URHO3D_HANDLER(Urho_Map_Editor, handle_input_event));
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(Urho_Map_Editor, handle_post_render_update));
    SubscribeToEvent(E_LOGMESSAGE, URHO3D_HANDLER(Urho_Map_Editor, handle_log_message));

    create_visuals();
}

Urho_Map_Editor::~Urho_Map_Editor()
{
    camera_controller_->release();
    context_->RemoveSubsystem<Editor_Camera_Controller>();
}

void Urho_Map_Editor::handle_log_message(StringHash event_type, VariantMap & event_data)
{
    String msg = event_data[LogMessage::P_MESSAGE].GetString();
    int state = event_data[LogMessage::P_LEVEL].GetInt();
    QString color;
    switch(state)
    {
        case(LOG_TRACE):
            color = "blue";
            break;
        case(LOG_WARNING):
            color = "yellow";
            break;
        case(LOG_ERROR):
            color = "red";
            break;
        case(LOG_DEBUG):
            color = "white";
            break;
        case(LOG_INFO):
            color = "green";
            break;
    }
    QString formatted_str("<font color=" + color + ">" + msg.CString() + "</font>");
    bbtk.ui->console->append(formatted_str);
}

void Urho_Map_Editor::handle_post_render_update(StringHash event_type, VariantMap & event_data)
{
    if (draw_debug_)
    {
        if (scene_ != nullptr)
        {
            PhysicsWorld * pw = scene_->GetComponent<PhysicsWorld>();
            if (pw != nullptr)
                pw->DrawDebugGeometry(false);
            Octree * oc = scene_->GetComponent<Octree>();
            if (oc != nullptr)
                oc->DrawDebugGeometry(true);
            Editor_Selection_Controller * esc = scene_->GetComponent<Editor_Selection_Controller>();
            if (esc != nullptr)
                esc->DrawDebugGeometry(true);
            Hex_Tile_Grid * tg = scene_->GetComponent<Hex_Tile_Grid>();
            if (tg != nullptr)
                tg->DrawDebugGeometry(false);
        }
    }
}

Scene * Urho_Map_Editor::get_active_scene()
{
    return scene_;
}   

void Urho_Map_Editor::set_active_scene(Scene * scene)
{
    scene_ = scene;
}


void Urho_Map_Editor::create_visuals()
{
    Graphics * graphics = GetSubsystem<Graphics>();
    graphics->SetWindowTitle("Build and Battle");
    
    // Get default style
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    XMLFile * xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    // Create console
    Console * console = engine_->CreateConsole();
    console->SetDefaultStyle(xmlFile);
    console->GetBackground()->SetOpacity(0.8f);
    console->GetBackground()->SetOpacity(0.8f);

    // Create debug HUD.
    DebugHud * debugHud = engine_->CreateDebugHud();
    debugHud->SetDefaultStyle(xmlFile);

    UI * usi = GetSubsystem<UI>();
    UIElement * root = usi->GetRoot();
    Button * btn = new Button(context_);
    btn->SetName("Button1");
    btn->SetSize(IntVector2(200, 200));
    btn->SetColor(Color(0.7f, 0.6f, 0.1f, 0.4f));
    root->AddChild(btn);

    //Button *
    Context * ctxt = GetContext();
    scene_ = new Scene(ctxt);
    scene_->CreateComponent<DebugRenderer>();
    scene_->CreateComponent<Octree>();
    PhysicsWorld * phys = scene_->CreateComponent<PhysicsWorld>();
    Hex_Tile_Grid * tg = scene_->CreateComponent<Hex_Tile_Grid>();
    Editor_Selection_Controller * editor_selection =
        scene_->CreateComponent<Editor_Selection_Controller>();
    phys->SetGravity(fvec3(0.0f, 0.0f, -9.81f));

    Node * editor_cam_node = new Node(ctxt);
    Camera * editor_cam = editor_cam_node->CreateComponent<Camera>();
    editor_cam_node->SetPosition(Vector3(8, -8, 5));
    editor_cam_node->SetDirection(Vector3(0, 0, -1));
    editor_cam_node->Pitch(-70.0f);

    Renderer * rnd = GetSubsystem<Renderer>();

    Viewport * vp = new Viewport(ctxt, scene_, editor_cam);
    vp->SetDrawDebug(true);
    rnd->SetViewport(0, vp);

    RenderPath * rp = new RenderPath;
    rp->Load(cache->GetResource<XMLFile>("RenderPaths/DeferredWithOutlines.xml"));
    vp->SetRenderPath(rp);

    camera_controller_->add_viewport(0);
    editor_selection->add_viewport(0);
    camera_controller_->set_camera(editor_cam);
    editor_selection->set_camera(editor_cam);

    Node * skyNode = scene_->CreateChild("Sky");
    skyNode->Rotate(Quaternion(90.0f, Vector3(1, 0, 0)));
    Skybox * skybox = skyNode->CreateComponent<Skybox>();
    skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox.xml"));

    // Create a directional light
    Node * light_node = scene_->CreateChild("Dir_Light");
    light_node->SetDirection(Vector3(-0.0f, -0.5f, -1.0f));
    Light * light = light_node->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetColor(Color(1.0f, 1.0f, 1.0f));
    light->SetSpecularIntensity(5.0f);
    light->SetBrightness(1.0f);

    Technique * tech = cache->GetResource<Technique>("Techniques/DiffNormal.xml");
    Technique * tech_outline = cache->GetResource<Technique>("Techniques/DiffNormalOutline.xml");

    // Create StaticModelGroups in the scene
    StaticModelGroup * lastGroup = nullptr;
    Material * grass_tile = cache->GetResource<Material>("Materials/Tiles/Grass.xml");
    Material * grass_tile_selected =
        cache->GetResource<Material>("Materials/Tiles/GrassSelected.xml");
    grass_tile_selected->SetShaderParameter("OutlineEnable", true);

    //<parameter name="OutlineWidth" value="0.01" />
    //<parameter name="OutlineColor" value="1 1 0 1" />
    Model * mod = cache->GetResource<Model>("Models/Tiles/Grass.mdl");

    Input_Context * in_context = input_map_->create_context("global_context");
    camera_controller_->setup_input_context(in_context);
    editor_selection->setup_input_context(in_context);
    input_translator_->push_context(in_context);
    setup_global_keys(in_context);

    int cnt = 0;
    for (int y = 0; y < 20; ++y)
    {
        for (int x = 0; x < 20; ++x)
        {
            for (int z = 0; z < 2; ++z)
            {
                Node * tile_node = scene_->CreateChild("Grass_Tile");

                StaticModel * modc = tile_node->CreateComponent<StaticModel>();
                modc->SetModel(mod);
                modc->SetMaterial(grass_tile);

                Editor_Selector * sel = tile_node->CreateComponent<Editor_Selector>();
                sel->SetModel(mod);
                sel->SetMaterial(grass_tile_selected);
                sel->set_selected(false);

                CollisionShape * cs = tile_node->CreateComponent<CollisionShape>();
                const BoundingBox & bb = modc->GetBoundingBox();
                cs->SetBox(bb.Size());

                // RigidBody * rb = tile_node->CreateComponent<RigidBody>();
                // rb->SetMass(0.0f);
                // if (z == 1)
                //     rb->SetMass(10.0f);
                // rb->SetFriction(0.1f);
                // rb->SetRestitution(0.9f);

                Tile_Occupier * occ = tile_node->CreateComponent<Tile_Occupier>();
                tile_node->AddListener(occ);

                tile_node->SetPosition(Hex_Tile_Grid::grid_to_world(ivec3(x, y, z)));
                tile_node->SetRotation(Quaternion(90.0f, fvec3(1.0f, 0.0f, 0.0f)));

                tile_node->AddTag("Scooby");
                tile_node->SetVar("Steve", IntVector2(1,2));
                tile_node->SetVar("Smackem", Vector3(1.1,2.2,3.3));
                tile_node->SetVar("Super", Vector4(1.1,2.2,3.3,4.4));


                ++cnt;
            }
        }
    }
}

void Urho_Map_Editor::setup_global_keys(Input_Context * ctxt)
{
    Input_Context * input_ctxt = input_map_->get_context(StringHash("global_context"));

    if (ctxt == nullptr)
        return;

    Input_Action_Trigger it;

    it.condition_.mouse_button_ = 0;
    it.mb_required_ = 0;
    it.mb_allowed_ = MOUSEB_ANY;
    it.qual_required_ = 0;
    it.qual_allowed_ = QUAL_ANY;
    it.condition_.key_ = KEY_F1;
    it.name_ = "ToggleConsole";
    it.trigger_state_ = T_BEGIN;
    ctxt->create_trigger(it);

    it.condition_.key_ = KEY_F2;
    it.name_ = "ToggleDebugHUD";
    ctxt->create_trigger(it);

    it.condition_.key_ = KEY_F9;
    it.name_ = "TakeScreenshot";
    ctxt->create_trigger(it);

    it.condition_.key_ = KEY_F3;
    it.name_ = "ToggleDebugGeometry";
    ctxt->create_trigger(it);

    it.condition_.key_ = KEY_DELETE;
    it.name_ = "DeleteSelection";
    ctxt->create_trigger(it);
}

void Urho_Map_Editor::handle_scene_update(StringHash /*event_type*/, VariantMap & event_data)
{
    bbtk.ui->details->update_node();
}

void Urho_Map_Editor::handle_input_event(StringHash event_type, VariantMap & event_data)
{
    StringHash name = event_data[InputTrigger::P_TRIGGER_NAME].GetStringHash();
    int state = event_data[InputTrigger::P_TRIGGER_STATE].GetInt();
    Vector2 norm_mpos = event_data[InputTrigger::P_NORM_MPOS].GetVector2();
    Vector2 norm_mdelta = event_data[InputTrigger::P_NORM_MDELTA].GetVector2();
    int wheel = event_data[InputTrigger::P_MOUSE_WHEEL].GetInt();

    if (name == StringHash("ToggleDebugHUD"))
    {
        GetSubsystem<DebugHud>()->ToggleAll();
    }
    else if (name == StringHash("DeleteSelection"))
    {
        bbtk.ui->details->clear_selection();
        scene_->GetComponent<Editor_Selection_Controller>()->delete_selection();
    }
    else if (name == StringHash("ToggleConsole"))
    {
        GetSubsystem<Console>()->Toggle();
    }
    else if (name == StringHash("ToggleDebugGeometry"))
    {
        draw_debug_ = !draw_debug_;
    }
    else if (name == StringHash("TakeScreenshot"))
    {
        Graphics * graphics = GetSubsystem<Graphics>();
        Image screenshot(context_);
        graphics->TakeScreenShot(screenshot);
        // Here we save in the Data folder with date and time appended
        screenshot.SavePNG(
            GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Screenshot_" +
            Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_') + ".png");
    }
}
