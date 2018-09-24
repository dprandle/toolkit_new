#include <editor_selection.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/OctreeQuery.h>
#include <Urho3D/Core/Context.h>
#include <input_translator.h>
#include <Urho3D/Core/CoreEvents.h>
#include <toolkit.h>
#include <Urho3D/Graphics/StaticModelGroup.h>

using namespace Urho3D;

EditorSelection::EditorSelection(Urho3D::Context * context)
    : Object(context), scn(nullptr), cam_node(nullptr)
{
    context->RegisterSubsystem(this);
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(EditorSelection, update));
    SubscribeToEvent(E_INPUT_TRIGGER, URHO3D_HANDLER(EditorSelection, handle_input_event));
}

EditorSelection::~EditorSelection()
{}

void EditorSelection::update(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data)
{}

void EditorSelection::handle_input_event(Urho3D::StringHash event_type,
                                         Urho3D::VariantMap & event_data)
{
    StringHash name = event_data[InputTrigger::P_TRIGGER_NAME].GetStringHash();
    int state = event_data[InputTrigger::P_TRIGGER_STATE].GetInt();
    Vector2 norm_mpos = event_data[InputTrigger::P_NORM_MPOS].GetVector2();
    Vector2 norm_mdelta = event_data[InputTrigger::P_NORM_MDELTA].GetVector2();
    int wheel = event_data[InputTrigger::P_MOUSE_WHEEL].GetInt();

    if (name == StringHash("SelectObject"))
    {
        Camera * cam = cam_node->GetComponent<Camera>();
        Octree * oct = scn->GetComponent<Octree>();
        PODVector<RayQueryResult> res;
        RayOctreeQuery q(res, cam->GetScreenRay(norm_mpos.x_, norm_mpos.y_));
        oct->RaycastSingle(q);

        for (int i = 0; i < res.Size(); ++i)
        {
            RayQueryResult & cr = res[i];
            bbtk.output_view()->writeToScreen(cr.node_->GetName().CString());

            StaticModelGroup * smg = cr.node_->GetComponent<StaticModelGroup>();

            if (smg != nullptr)
            {
                Node * nd = smg->GetInstanceNode(cr.subObject_);
                if (nd != nullptr)
                {
                    bbtk.output_view()->writeToScreen(QString::number(cr.subObject_) + ": " +
                                                      QString(nd->GetName().CString()));
                    auto ires = selection.insert(nd);
//                    if (ires.second)
                }
            }
        }
    }
}

void EditorSelection::setup_input_context(input_context * ctxt)
{
    trigger_condition tc;
    input_action_trigger * it;

    tc.key = 0;
    tc.mouse_button = MOUSEB_LEFT;
    it = ctxt->create_trigger(tc);
    it->name = "SelectObject";
    it->trigger_state = t_begin;
    it->mb_required = 0;
    it->mb_allowed = 0;
    it->qual_required = 0;
    it->qual_allowed = QUAL_ANY;
}

void EditorSelection::set_scene(Urho3D::Scene * scn_)
{
    scn = scn_;
}

void EditorSelection::set_camera(Urho3D::Node * node_)
{
    cam_node = node_;
}
