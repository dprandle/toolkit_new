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
#include <Urho3D/IO/IOEvents.h>
#include <Urho3D/IO/Log.h>

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

Urho_Map_Editor::Urho_Map_Editor(Urho3D::Context * context, void * window_id)
    : Urho_Window(context, window_id),
      scene_(nullptr),
      cam_node_(nullptr),
      camera_controller_(new Editor_Camera_Controller(context)),
      draw_debug_(false)
{
    Urho3D::VariantMap engine_params;
    engine_params[Urho3D::EP_FRAME_LIMITER] = false;
    engine_params[Urho3D::EP_LOG_NAME] = GetSubsystem<Urho3D::FileSystem>()->GetProgramDir() + "BBToolkit.log";
    engine_params[Urho3D::EP_FULL_SCREEN] = false;
    engine_params[Urho3D::EP_WINDOW_WIDTH] = 1280;
    engine_params[Urho3D::EP_WINDOW_HEIGHT] = 720;
    engine_params[Urho3D::EP_WINDOW_RESIZABLE] = true;
    engine_params[Urho3D::EP_EXTERNAL_WINDOW] = window_id;

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

    SubscribeToEvent(Urho3D::E_SCENEUPDATE, URHO3D_HANDLER(Urho_Map_Editor, handle_scene_update));
    SubscribeToEvent(Urho3D::E_INPUT_TRIGGER, URHO3D_HANDLER(Urho_Map_Editor, handle_input_event));
    SubscribeToEvent(Urho3D::E_POSTRENDERUPDATE, URHO3D_HANDLER(Urho_Map_Editor, handle_post_render_update));
    SubscribeToEvent(Urho3D::E_LOGMESSAGE, URHO3D_HANDLER(Urho_Map_Editor, handle_log_message));

    create_visuals();
}

Urho_Map_Editor::~Urho_Map_Editor()
{
    camera_controller_->release();
    context_->RemoveSubsystem<Editor_Camera_Controller>();
}

void Urho_Map_Editor::handle_log_message(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data)
{
    String msg = event_data[Urho3D::LogMessage::P_MESSAGE].GetString();
    int state = event_data[Urho3D::LogMessage::P_LEVEL].GetInt();
    QString color;
    switch(state)
    {
        case(Urho3D::LOG_TRACE):
            color = "blue";
            break;
        case(Urho3D::LOG_WARNING):
            color = "yellow";
            break;
        case(Urho3D::LOG_ERROR):
            color = "red";
            break;
        case(Urho3D::LOG_DEBUG):
            color = "white";
            break;
        case(Urho3D::LOG_INFO):
            color = "green";
            break;
    }
    QString formatted_str("<font color=" + color + ">" + msg.CString() + "</font>");
    bbtk.ui->console->append(formatted_str);
}

void Urho_Map_Editor::handle_post_render_update(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data)
{
    if (draw_debug_)
    {
        if (scene_ != nullptr)
        {
            Urho3D::PhysicsWorld * pw = scene_->GetComponent<Urho3D::PhysicsWorld>();
            if (pw != nullptr)
                pw->DrawDebugGeometry(false);
            Urho3D::Octree * oc = scene_->GetComponent<Urho3D::Octree>();
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

void Urho_Map_Editor::create_visuals()
{
    Urho3D::Graphics * graphics = GetSubsystem<Urho3D::Graphics>();
    graphics->SetWindowTitle("Build and Battle");

    // Get default style
    Urho3D::ResourceCache * cache = GetSubsystem<Urho3D::ResourceCache>();
    Urho3D::XMLFile * xmlFile = cache->GetResource<Urho3D::XMLFile>("UI/DefaultStyle.xml");

    // Create console
    Urho3D::Console * console = engine_->CreateConsole();
    console->SetDefaultStyle(xmlFile);
    console->GetBackground()->SetOpacity(0.8f);
    console->GetBackground()->SetOpacity(0.8f);

    // Create debug HUD.
    Urho3D::DebugHud * debugHud = engine_->CreateDebugHud();
    debugHud->SetDefaultStyle(xmlFile);

    Urho3D::UI * usi = GetSubsystem<Urho3D::UI>();
    Urho3D::UIElement * root = usi->GetRoot();
    Urho3D::Button * btn = new Urho3D::Button(context_);
    btn->SetName("Button1");
    btn->SetSize(Urho3D::IntVector2(200, 200));
    btn->SetColor(Color(0.7f, 0.6f, 0.1f, 0.4f));
    root->AddChild(btn);

    //Button *
    Urho3D::Context * ctxt = GetContext();
    scene_ = new Urho3D::Scene(ctxt);
    scene_->CreateComponent<Urho3D::DebugRenderer>();
    scene_->CreateComponent<Urho3D::Octree>();
    Urho3D::PhysicsWorld * phys = scene_->CreateComponent<Urho3D::PhysicsWorld>();
    Hex_Tile_Grid * tg = scene_->CreateComponent<Hex_Tile_Grid>();
    Editor_Selection_Controller * editor_selection =
        scene_->CreateComponent<Editor_Selection_Controller>();
    phys->SetGravity(fvec3(0.0f, 0.0f, -9.81f));

    Urho3D::Node * editor_cam_node = new Urho3D::Node(ctxt);
    Urho3D::Camera * editor_cam = editor_cam_node->CreateComponent<Urho3D::Camera>();
    editor_cam_node->SetPosition(Urho3D::Vector3(8, -8, 5));
    editor_cam_node->SetDirection(Urho3D::Vector3(0, 0, -1));
    editor_cam_node->Pitch(-70.0f);

    Urho3D::Renderer * rnd = GetSubsystem<Urho3D::Renderer>();

    Urho3D::Viewport * vp = new Urho3D::Viewport(ctxt, scene_, editor_cam);
    vp->SetDrawDebug(true);
    rnd->SetViewport(0, vp);

    Urho3D::RenderPath * rp = new Urho3D::RenderPath;
    rp->Load(cache->GetResource<Urho3D::XMLFile>("RenderPaths/DeferredWithOutlines.xml"));
    vp->SetRenderPath(rp);

    camera_controller_->add_viewport(0);
    editor_selection->add_viewport(0);
    camera_controller_->set_camera(editor_cam);
    editor_selection->set_camera(editor_cam);

    Urho3D::Node * skyNode = scene_->CreateChild("Sky");
    skyNode->Rotate(Urho3D::Quaternion(90.0f, Urho3D::Vector3(1, 0, 0)));
    Urho3D::Skybox * skybox = skyNode->CreateComponent<Urho3D::Skybox>();
    skybox->SetModel(cache->GetResource<Urho3D::Model>("Models/Box.mdl"));
    skybox->SetMaterial(cache->GetResource<Urho3D::Material>("Materials/Skybox.xml"));

    // Create a directional light
    Urho3D::Node * light_node = scene_->CreateChild("Dir_Light");
    light_node->SetDirection(Urho3D::Vector3(-0.0f, -0.5f, -1.0f));
    Urho3D::Light * light = light_node->CreateComponent<Urho3D::Light>();
    light->SetLightType(Urho3D::LIGHT_DIRECTIONAL);
    light->SetColor(Color(1.0f, 1.0f, 1.0f));
    light->SetSpecularIntensity(5.0f);
    light->SetBrightness(1.0f);

    Urho3D::Technique * tech = cache->GetResource<Urho3D::Technique>("Techniques/DiffNormal.xml");
    Urho3D::Technique * tech_outline = cache->GetResource<Urho3D::Technique>("Techniques/DiffNormalOutline.xml");

    // Create StaticModelGroups in the scene
    Urho3D::StaticModelGroup * lastGroup = nullptr;
    Urho3D::Material * grass_tile = cache->GetResource<Urho3D::Material>("Materials/Tiles/Grass.xml");
    Urho3D::Material * grass_tile_selected =
        cache->GetResource<Urho3D::Material>("Materials/Tiles/GrassSelected.xml");
    grass_tile_selected->SetShaderParameter("OutlineEnable", true);

    //<parameter name="OutlineWidth" value="0.01" />
    //<parameter name="OutlineColor" value="1 1 0 1" />
    Urho3D::Model * mod = cache->GetResource<Urho3D::Model>("Models/Tiles/Grass.mdl");

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
                Urho3D::Node * tile_node = scene_->CreateChild("Grass_Tile_" + String(cnt));

                Urho3D::StaticModel * modc = tile_node->CreateComponent<Urho3D::StaticModel>();
                modc->SetModel(mod);
                modc->SetMaterial(grass_tile);

                Editor_Selector * sel = tile_node->CreateComponent<Editor_Selector>();
                sel->SetModel(mod);
                sel->SetMaterial(grass_tile_selected);
                sel->set_selected(false);

                Urho3D::CollisionShape * cs = tile_node->CreateComponent<Urho3D::CollisionShape>();
                const Urho3D::BoundingBox & bb = modc->GetBoundingBox();
                cs->SetBox(bb.Size());

                Urho3D::RigidBody * rb = tile_node->CreateComponent<Urho3D::RigidBody>();
                rb->SetMass(0.0f);
                // if (z == 1)
                //     rb->SetMass(10.0f);
                // rb->SetFriction(0.1f);
                // rb->SetRestitution(0.9f);

                Tile_Occupier * occ = tile_node->CreateComponent<Tile_Occupier>();
                tile_node->AddListener(occ);

                tile_node->SetPosition(Hex_Tile_Grid::grid_to_world(ivec3(x, y, z)));
                tile_node->SetRotation(Urho3D::Quaternion(90.0f, fvec3(1.0f, 0.0f, 0.0f)));

                //lastGroup->AddInstanceNode(tile_node);
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

    it.condition_.key_ = Urho3D::KEY_ESCAPE;
    it.condition_.mouse_button_ = 0;
    it.name_ = "CloseWindow";
    it.trigger_state_ = T_END;
    it.mb_required_ = 0;
    it.mb_allowed_ = Urho3D::MOUSEB_ANY;
    it.qual_required_ = 0;
    it.qual_allowed_ = Urho3D::QUAL_ANY;
    ctxt->create_trigger(it);

    it.condition_.key_ = Urho3D::KEY_F1;
    it.name_ = "ToggleConsole";
    it.trigger_state_ = T_BEGIN;
    ctxt->create_trigger(it);

    it.condition_.key_ = Urho3D::KEY_F2;
    it.name_ = "ToggleDebugHUD";
    ctxt->create_trigger(it);

    it.condition_.key_ = Urho3D::KEY_F9;
    it.name_ = "TakeScreenshot";
    ctxt->create_trigger(it);

    it.condition_.key_ = Urho3D::KEY_F3;
    it.name_ = "ToggleDebugGeometry";
    ctxt->create_trigger(it);
}

void Urho_Map_Editor::handle_scene_update(Urho3D::StringHash /*event_type*/, Urho3D::VariantMap & event_data)
{
    bbtk.ui->details->update_node();
}

void Urho_Map_Editor::handle_input_event(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data)
{
    StringHash name = event_data[Urho3D::InputTrigger::P_TRIGGER_NAME].GetStringHash();
    int state = event_data[Urho3D::InputTrigger::P_TRIGGER_STATE].GetInt();
    Urho3D::Vector2 norm_mpos = event_data[Urho3D::InputTrigger::P_NORM_MPOS].GetVector2();
    Urho3D::Vector2 norm_mdelta = event_data[Urho3D::InputTrigger::P_NORM_MDELTA].GetVector2();
    int wheel = event_data[Urho3D::InputTrigger::P_MOUSE_WHEEL].GetInt();

    if (name == StringHash("CloseWindow"))
    {
        Urho3D::Console * console = GetSubsystem<Urho3D::Console>();
        if (console->IsVisible())
            console->SetVisible(false);
        else
            engine_->Exit();
    }
    else if (name == StringHash("ToggleDebugHUD"))
    {
        GetSubsystem<Urho3D::DebugHud>()->ToggleAll();
    }
    else if (name == StringHash("ToggleConsole"))
    {
        GetSubsystem<Urho3D::Console>()->Toggle();
    }
    else if (name == StringHash("ToggleDebugGeometry"))
    {
        draw_debug_ = !draw_debug_;
    }
    else if (name == StringHash("TakeScreenshot"))
    {
        Urho3D::Graphics * graphics = GetSubsystem<Urho3D::Graphics>();
        Urho3D::Image screenshot(context_);
        graphics->TakeScreenShot(screenshot);
        // Here we save in the Data folder with date and time appended
        screenshot.SavePNG(
            GetSubsystem<Urho3D::FileSystem>()->GetProgramDir() + "Data/Screenshot_" +
            Urho3D::Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_') + ".png");
    }
}
