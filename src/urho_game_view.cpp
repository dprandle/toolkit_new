#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/BorderImage.h>

#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsEvents.h>

#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Resource/ResourceCache.h>

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/EngineDefs.h>

#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>

#include <Urho3D/Audio/Audio.h>

#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Scene/Scene.h>

#include <Urho3D/IO/FileSystem.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/Core/CoreEvents.h>

#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/StaticModelGroup.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Technique.h>

#include <urho_game_view.h>
#include <mtdebug_print.h>
#include <tile_occupier.h>
#include <hex_tile_grid.h>
#include <editor_selector.h>
#include <editor_selection_controller.h>
#include <editor_camera_controller.h>
#include <input_translator.h>
#include <toolkit.h>
#include <component_widget.h>

using namespace Urho3D;

Urho_Game_View::Urho_Game_View(Context * context, void * window_id)
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

    input_translator_->init();
    camera_controller_->init();

    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(Urho_Game_View, handle_scene_update));
    SubscribeToEvent(E_INPUT_TRIGGER, URHO3D_HANDLER(Urho_Game_View, handle_input_event));
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(Urho_Game_View, handle_post_render_update));

    create_visuals();
}

Urho_Game_View::~Urho_Game_View()
{
    camera_controller_->release();
    context_->RemoveSubsystem<Editor_Camera_Controller>();
}

void Urho_Game_View::handle_post_render_update(StringHash event_type, VariantMap & event_data)
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

void Urho_Game_View::create_visuals()
{
    ResourceCache * cache = GetSubsystem<ResourceCache>();

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

    cam_node_ = new Node(ctxt);
    Camera * editor_cam = cam_node_->CreateComponent<Camera>();
    cam_node_->SetPosition(Vector3(8, -8, 5));
    cam_node_->SetDirection(Vector3(0, 0, -1));
    cam_node_->Pitch(-70.0f);

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
    dir_light_node_ = scene_->CreateChild("Dir_Light");
    dir_light_node_->SetDirection(Vector3(-0.0f, -0.5f, -1.0f));
    dir_light_node_->SetPosition(Vector3(5,5,5));
    Light * light = dir_light_node_->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetColor(Color(1.0f, 1.0f, 1.0f));
    light->SetSpecularIntensity(5.0f);
    light->SetBrightness(1.0f);

    Technique * tech = cache->GetResource<Technique>("Techniques/DiffNormal.xml");
    Technique * tech_outline = cache->GetResource<Technique>("Techniques/DiffNormalOutline.xml");

    Material * grass_tile = cache->GetResource<Material>("Materials/Tiles/Grass.xml");    
    Material * grass_tile_selected =
        cache->GetResource<Material>("Materials/Tiles/GrassSelected.xml");
    grass_tile_selected->SetShaderParameter("OutlineEnable", true);

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
                Node * tile_node = scene_->CreateChild("Grass_Tile_" + String(cnt));

                StaticModel * modc = tile_node->CreateComponent<StaticModel>();
                modc->SetCastShadows(true);
                modc->SetModel(mod);
                modc->SetMaterial(grass_tile);

                Editor_Selector * sel = tile_node->CreateComponent<Editor_Selector>();
                sel->set_selection_material("Materials/Tiles/GrassSelected.xml");
                sel->set_selected(tile_node, false);

                CollisionShape * cs = tile_node->CreateComponent<CollisionShape>();
                const BoundingBox & bb = modc->GetBoundingBox();
                cs->SetBox(bb.Size());

                RigidBody * rb = tile_node->CreateComponent<RigidBody>();
                rb->SetMass(0.0f);

                Tile_Occupier * occ = tile_node->CreateComponent<Tile_Occupier>();
                tile_node->AddListener(occ);

                tile_node->SetPosition(Hex_Tile_Grid::grid_to_world(ivec3(x, y, z)));
                tile_node->SetRotation(Quaternion(90.0f, fvec3(1.0f, 0.0f, 0.0f)));
                ++cnt;
            }
        }
    }



    VariantMap nested1;
    nested1["String"] = "Wow!";
    nested1["Double"] = 2023.34;
    nested1["Float"] = 222.0f;
    nested1["BoolFalse"] = false;

    VariantVector v;
    v.Push("Value");
    v.Push(2);
    v.Push(20000.0f);
    v.Push(nested1);

    IntRect ex_rect;
    ex_rect.left_ = 3;
    ex_rect.right_ = 10;
    ex_rect.bottom_ = 5;
    ex_rect.top_ =16;

    IntVector2 v2;
    v2.x_ = 3;
    v2.y_ = -5;


    Vector2 vf2;
    vf2.x_ = 4.67;
    vf2.y_ = 6.88;

    dir_light_node_->SetVar("IntRect", ex_rect);
    dir_light_node_->SetVar("IntVec2", v2);
    dir_light_node_->SetVar("Vec2", vf2);
    dir_light_node_->SetVar("String", "4DSF");
    dir_light_node_->SetVar("Int", 2);
    dir_light_node_->SetVar("Float", 5.0f);
    dir_light_node_->SetVar("Double", 2.0);
    dir_light_node_->SetVar("BoolTrue", true);
    dir_light_node_->SetVar("BoolFalse", false);
    dir_light_node_->SetVar("NestedObj", nested1);
    dir_light_node_->SetVar("IntVec3", IntVector3(2,2,3));

    dir_light_node_->AddTag("Scooby");
    dir_light_node_->AddTag("Yeppers");
}

void Urho_Game_View::setup_global_keys(Input_Context * ctxt)
{
    Input_Context * input_ctxt = input_map_->get_context(StringHash("global_context"));

    if (ctxt == nullptr)
        return;

    Input_Action_Trigger it;

    it.condition_.key_ = KEY_ESCAPE;
    it.condition_.mouse_button_ = 0;
    it.name_ = "CloseWindow";
    it.trigger_state_ = T_END;
    it.mb_required_ = 0;
    it.mb_allowed_ = MOUSEB_ANY;
    it.qual_required_ = 0;
    it.qual_allowed_ = QUAL_ANY;
    ctxt->create_trigger(it);

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
}

void Urho_Game_View::handle_scene_update(StringHash /*event_type*/, VariantMap & event_data)
{

}

void Urho_Game_View::handle_input_event(StringHash event_type, VariantMap & event_data)
{
    StringHash name = event_data[InputTrigger::P_TRIGGER_NAME].GetStringHash();
    int state = event_data[InputTrigger::P_TRIGGER_STATE].GetInt();
    Vector2 norm_mpos = event_data[InputTrigger::P_NORM_MPOS].GetVector2();
    Vector2 norm_mdelta = event_data[InputTrigger::P_NORM_MDELTA].GetVector2();
    int wheel = event_data[InputTrigger::P_MOUSE_WHEEL].GetInt();

    if (name == StringHash("CloseWindow"))
    {
        Console * console = GetSubsystem<Console>();
        if (console->IsVisible())
            console->SetVisible(false);
        else
            engine_->Exit();
    }
    else if (name == StringHash("ToggleDebugHUD"))
    {
        GetSubsystem<DebugHud>()->ToggleAll();
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
