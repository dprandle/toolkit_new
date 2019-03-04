#include <editor_selection_controller.h>
#include <input_translator.h>
#include <mtdebug_print.h>
#include <hex_tile_grid.h>
#include <tile_occupier.h>
#include <Urho3D/Scene/SceneEvents.h>

#include <Urho3D/Graphics/DebugRenderer.h>

#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Material.h>

#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/Scene.h>

#include <Urho3D/Math/Frustum.h>

#include <Urho3D/Resource/ResourceCache.h>

#include <Urho3D/Graphics/View.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/StaticModelGroup.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/OctreeQuery.h>

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/Context.h>

#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/BorderImage.h>

#include <toolkit.h>
#include <ui_toolkit.h>
#include <details_view.h>
#include <editor_selector.h>
#include <string>

using namespace Urho3D;

Editor_Selection_Controller::Editor_Selection_Controller(Urho3D::Context * context)
    : Component(context),
      cam_comp_(nullptr),
      scene_(nullptr),
      selection_rect_(-1.0f, -1.0f, -1.0f, -1.0f),
      movement_flag_(0),
      move_allowed_(true),
      do_snap_(false)
{
    UI * usi = GetSubsystem<UI>();

    ui_root_ = usi->GetRoot();

    ui_selection_rect_ = ui_root_->CreateChild<UIElement>("sel_rect_root");
    ui_selection_rect_->SetVisible(false);
    //ui_selection_rect->SetSortChildren(false);

    BorderImage * ui_left_border_sel_rect =
        ui_selection_rect_->CreateChild<BorderImage>("leftb_selection_rect");
    ui_left_border_sel_rect->SetEnableAnchor(true);
    ui_left_border_sel_rect->SetMinAnchor(0.0f, 0.0f);
    ui_left_border_sel_rect->SetMaxAnchor(0.0f, 1.0f);
    ui_left_border_sel_rect->SetMaxOffset(ivec2(BORDER_SIZE, 0));
    ui_left_border_sel_rect->SetColor(SEL_RECT_BORDER_COL);

    BorderImage * ui_right_border_sel_rect =
        ui_selection_rect_->CreateChild<BorderImage>("rightb_selection_rect");
    ui_right_border_sel_rect->SetEnableAnchor(true);
    ui_right_border_sel_rect->SetMinAnchor(1.0f, 0.0f);
    ui_right_border_sel_rect->SetMaxAnchor(1.0f, 1.0f);
    ui_right_border_sel_rect->SetMinOffset(ivec2(0, 0));
    ui_right_border_sel_rect->SetMaxOffset(ivec2(BORDER_SIZE, 0));
    ui_right_border_sel_rect->SetColor(SEL_RECT_BORDER_COL);

    BorderImage * ui_top_border_sel_rect =
        ui_selection_rect_->CreateChild<BorderImage>("topb_selection_rect");
    ui_top_border_sel_rect->SetEnableAnchor(true);
    ui_top_border_sel_rect->SetMinAnchor(0.0f, 0.0f);
    ui_top_border_sel_rect->SetMaxAnchor(1.0f, 0.0f);
    ui_top_border_sel_rect->SetMinOffset(ivec2(BORDER_SIZE, 0));
    ui_top_border_sel_rect->SetMaxOffset(ivec2(-BORDER_SIZE, BORDER_SIZE));
    ui_top_border_sel_rect->SetColor(SEL_RECT_BORDER_COL);

    BorderImage * ui_bottom_border_sel_rect =
        ui_selection_rect_->CreateChild<BorderImage>("bottomb_selection_rect");
    ui_bottom_border_sel_rect->SetEnableAnchor(true);
    ui_bottom_border_sel_rect->SetMinAnchor(0.0f, 1.0f);
    ui_bottom_border_sel_rect->SetMaxAnchor(1.0f, 1.0f);
    ui_bottom_border_sel_rect->SetMinOffset(ivec2(BORDER_SIZE, 0));
    ui_bottom_border_sel_rect->SetMaxOffset(ivec2(-BORDER_SIZE, BORDER_SIZE));
    ui_bottom_border_sel_rect->SetColor(SEL_RECT_BORDER_COL);

    BorderImage * ui_inner_sel_rect =
        ui_selection_rect_->CreateChild<BorderImage>("inner_selection_rect");
    ui_inner_sel_rect->SetEnableAnchor(true);
    ui_inner_sel_rect->SetMinAnchor(0.0f, 0.0f);
    ui_inner_sel_rect->SetMaxAnchor(1.0f, 1.0f);
    ui_inner_sel_rect->SetMinOffset(ivec2(BORDER_SIZE, BORDER_SIZE));
    ui_inner_sel_rect->SetMaxOffset(ivec2(-BORDER_SIZE, -BORDER_SIZE));
    ui_inner_sel_rect->SetColor(SEL_RECT_COL);

    hashes_.Push(StringHash(SEL_OBJ_NAME));
    hashes_.Push(StringHash(DRAG_SELECTED_OBJECT));
    hashes_.Push(StringHash(EXTEND_SEL_OBJ_NAME));
    hashes_.Push(StringHash(DRAGGING));
    hashes_.Push(StringHash(ENABLE_SELECTION_RECT));
    hashes_.Push(StringHash(Z_MOVE_HELD));
    hashes_.Push(StringHash(X_MOVE_HELD));
    hashes_.Push(StringHash(Y_MOVE_HELD));
    hashes_.Push(StringHash(TOGGLE_OCC_DEBUG));

    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Editor_Selection_Controller, handle_update));
    SubscribeToEvent(E_INPUT_TRIGGER,
                     URHO3D_HANDLER(Editor_Selection_Controller, handle_input_event));
    SubscribeToEvent(E_COMPONENTADDED,
                     URHO3D_HANDLER(Editor_Selection_Controller, handle_component_added));
    SubscribeToEvent(E_COMPONENTREMOVED,
                     URHO3D_HANDLER(Editor_Selection_Controller, handle_component_removed));
}

Editor_Selection_Controller::~Editor_Selection_Controller()
{}

void Editor_Selection_Controller::OnSceneSet(Urho3D::Scene * scene)
{
    scene_ = scene;
    Component::OnSceneSet(scene_);
}

void Editor_Selection_Controller::clear_selection()
{
    selection_.Clear();
    sel_rect_selection_.Clear();
}

void Editor_Selection_Controller::handle_component_added(Urho3D::StringHash event_type,
                                                         Urho3D::VariantMap & event_data)
{
    Scene * scn = static_cast<Scene *>(event_data[NodeRemoved::P_SCENE].GetPtr());
    if (scene_ != scn)
        return;
    Component * comp = static_cast<Component *>(event_data[ComponentRemoved::P_COMPONENT].GetPtr());
    if (comp->IsInstanceOf<Editor_Selector>())
        scene_sel_comps_.Insert(static_cast<Editor_Selector *>(comp));
}

void Editor_Selection_Controller::handle_component_removed(Urho3D::StringHash event_type,
                                                           Urho3D::VariantMap & event_data)
{
    Scene * scn = static_cast<Scene *>(event_data[ComponentRemoved::P_SCENE].GetPtr());
    if (scene_ != scn)
        return;
    Component * comp = static_cast<Component *>(event_data[ComponentRemoved::P_COMPONENT].GetPtr());
    if (comp->IsInstanceOf<Editor_Selector>())
        scene_sel_comps_.Erase(static_cast<Editor_Selector *>(comp));
}

void Editor_Selection_Controller::DrawDebugGeometry(bool depth_test)
{
    if (scene_ == nullptr)
        return;

    Urho3D::DebugRenderer * deb = scene_->GetComponent<DebugRenderer>();
    if (deb == nullptr)
        return;

    auto iter = cached_raycasts_.Begin();
    while (iter != cached_raycasts_.End())
    {
        Node * camn = cam_comp_->GetNode();
        fvec3 cam_pos = camn->GetPosition() + camn->GetDirection().Normalized() * 0.1f;
        fvec3 node_pos = iter->first_->GetPosition();
        deb->AddLine(cam_pos, node_pos, Color(1.0f, 0.0f, 0.0f), depth_test);
        ++iter;
    }
}

void Editor_Selection_Controller::remove_viewport(int vp_ind)
{
    viewports_.Remove(vp_ind);
}

void Editor_Selection_Controller::translate_selection(const fvec3 & translation)
{
    auto sel_iter = selection_.Begin();
    while (sel_iter != selection_.End())
    {
        (*sel_iter)->Translate(translation, TS_WORLD);
        ++sel_iter;
    }
}

void Editor_Selection_Controller::toggle_occ_debug_selection()
{
    auto sel_iter = selection_.Begin();
    while (sel_iter != selection_.End())
    {
        Tile_Occupier * toc = (*sel_iter)->GetComponent<Tile_Occupier>();
        if (toc != nullptr)
            toc->enable_debug(!toc->debug_enabled());
        ++sel_iter;
    }
}

void Editor_Selection_Controller::remove_from_selection(Urho3D::Node * node)
{
    auto iter = selection_.Begin();
    while (iter != selection_.End())
    {
        if (*iter == node)
        {
            selection_.Erase(iter);
            return;
        }
        ++iter;
    }
}

void Editor_Selection_Controller::set_camera(Urho3D::Camera * cam)
{
    cam_comp_ = cam;
}

Urho3D::Camera * Editor_Selection_Controller::get_camera()
{
    return cam_comp_;
}

void Editor_Selection_Controller::handle_update(Urho3D::StringHash event_type,
                                                Urho3D::VariantMap & event_data)
{
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    static HashMap<Material *, bool> mat_map;
    Urho3D::Vector<Hex_Tile_Grid::Tile_Item> allowed_items;
    Urho3D::Vector<Urho3D::Node*> sel_vec;

    allowed_items.Resize(selection_.Size());
    int i = 0;
    auto sel_iter_al = selection_.Begin();
    while (sel_iter_al != selection_.End())
    {
        int node_id = (*sel_iter_al)->GetID();
        allowed_items[i].node_id_ = node_id;
        sel_vec.Push(*sel_iter_al);
        ++i;
        ++sel_iter_al;
    }


    bbtk.ui->details->set_nodes(sel_vec);

    auto iter = selection_.Begin();
    while (iter != selection_.End())
    {
        Editor_Selector * es = (*iter)->GetComponent<Editor_Selector>();
        Material * mat = es->GetMaterial();

        auto fiter = mat_map.Find(mat);
        if (fiter == mat_map.End())
            mat_map[mat] = true;

        Hex_Tile_Grid * tg = scene_->GetComponent<Hex_Tile_Grid>();
        Tile_Occupier * occ = (*iter)->GetComponent<Tile_Occupier>();

        uint32_t node_id = (*iter)->GetID();
        (*iter)->Translate(frame_translation_, TS_WORLD);

        if (occ != nullptr && occ->IsEnabled())
        {
            auto occ_tiles =
                tg->occupied(occ->tile_spaces(), (*iter)->GetPosition(), allowed_items);
            mat_map[mat] = mat_map[mat] && occ_tiles.Empty();
        }
        ++iter;
    }
    total_drag_translation_ += frame_translation_;
    frame_translation_ = fvec3();

    if (do_snap_)
    {
        snap_selection();
        do_snap_ = false;
    }

    auto mat_iter = mat_map.Begin();
    while (mat_iter != mat_map.End())
    {
        move_allowed_ = true;
        fvec4 color(0.0f, 0.0f, 1.0f, 1.0f);
        if (!mat_iter->second_)
        {
            move_allowed_ = false;
            color = fvec4(1.0f, 0.0f, 0.0f, 1.0f);
            mat_iter->second_ = true;
        }

        mat_iter->first_->SetShaderParameter("OutlineColor", color);
        ++mat_iter;
    }

    auto sel_comp_iter = scene_sel_comps_.Begin();
    while (sel_comp_iter != scene_sel_comps_.End())
    {
        (*sel_comp_iter)->set_selected(is_selected((*sel_comp_iter)->GetNode()));
        ++sel_comp_iter;
    }
}

void Editor_Selection_Controller::snap_selection()
{
    auto sel_iter = selection_.Begin();
    while (sel_iter != selection_.End())
    {
        fvec3 pos = (*sel_iter)->GetPosition();
        Hex_Tile_Grid::snap_to_grid(pos);
        (*sel_iter)->SetPosition(pos);
        ++sel_iter;
    }
    sel_iter = sel_rect_selection_.Begin();
    while (sel_iter != sel_rect_selection_.End())
    {
        fvec3 pos = (*sel_iter)->GetPosition();
        Hex_Tile_Grid::snap_to_grid(pos);
        (*sel_iter)->SetPosition(pos);
        ++sel_iter;
    }
}

void Editor_Selection_Controller::add_viewport(int vp_ind)
{
    if (!viewports_.Contains(vp_ind))
        viewports_.Push(vp_ind);
}

void Editor_Selection_Controller::setup_input_context(Input_Context * ctxt)
{
    Input_Action_Trigger it;

    it.condition_.key_ = 0;
    it.condition_.mouse_button_ = MOUSEB_LEFT;
    it.trigger_state_ = T_BEGIN;
    it.mb_required_ = 0;
    it.mb_allowed_ = 0;
    it.qual_required_ = 0;
    it.qual_allowed_ = 0;
    it.name_ = SEL_OBJ_NAME;
    ctxt->create_trigger(it);

    it.name_ = DRAG_SELECTED_OBJECT;
    it.trigger_state_ = T_BEGIN | T_END;
    ctxt->create_trigger(it);

    it.name_ = ENABLE_SELECTION_RECT;
    it.qual_required_ = QUAL_SHIFT;
    ctxt->create_trigger(it);

    it.name_ = EXTEND_SEL_OBJ_NAME;
    it.trigger_state_ = T_BEGIN;
    it.qual_required_ = QUAL_CTRL;
    ctxt->create_trigger(it);

    it.condition_.mouse_button_ = MOUSEB_MOVE;
    it.name_ = DRAGGING;
    it.mb_required_ = MOUSEB_LEFT;
    it.qual_required_ = 0;
    it.qual_allowed_ = QUAL_ANY;
    ctxt->create_trigger(it);

    it.condition_.key_ = KEY_Z;
    it.condition_.mouse_button_ = 0;
    it.trigger_state_ = T_BEGIN | T_END;
    it.mb_required_ = 0;
    it.mb_allowed_ = MOUSEB_ANY;
    it.qual_required_ = 0;
    it.qual_allowed_ = 0;
    it.name_ = Z_MOVE_HELD;
    ctxt->create_trigger(it);

    it.condition_.key_ = KEY_X;
    it.name_ = X_MOVE_HELD;
    ctxt->create_trigger(it);

    it.condition_.key_ = KEY_Y;
    it.name_ = Y_MOVE_HELD;
    ctxt->create_trigger(it);

    it.condition_.key_ = KEY_O;
    it.trigger_state_ = T_BEGIN;
    it.name_ = TOGGLE_OCC_DEBUG;
    ctxt->create_trigger(it);
}

bool Editor_Selection_Controller::is_selected(Urho3D::Node * obj_node)
{
    auto fiter = selection_.Find(obj_node);
    if (fiter != selection_.End())
        return true;

    auto sq_iter = sel_rect_selection_.Find(obj_node);
    if (sq_iter != sel_rect_selection_.End())
        return true;
    
    return false;
}

void Editor_Selection_Controller::_add_to_selection_from_rect()
{
    Frustum f;
    Renderer * rnd = GetSubsystem<Renderer>();
    irect screct = rnd->GetViewport(0)->GetRect();

    ivec2 sz = ui_root_->GetSize();
    ivec2 sel_pos = ui_selection_rect_->GetPosition();
    ivec2 sel_sz = ui_selection_rect_->GetSize();

    fvec2 norm_sel_pos(float(sel_pos.x_) / float(sz.x_), float(sel_pos.y_) / float(sz.y_));
    fvec2 norm_sz(float(sel_sz.x_) / float(sz.x_), float(sel_sz.y_) / float(sz.y_));
    fvec2 norm_offset = norm_sel_pos + 0.5f * norm_sz - fvec2(0.5f, 0.5f);

    float fov = cam_comp_->GetFov();
    float ar = cam_comp_->GetAspectRatio();
    float near_z = cam_comp_->GetNearClip();

    float new_ar = Abs(float(sel_sz.x_) / float(sel_sz.y_));
    float new_fov = Abs(norm_sz.y_ * fov);

    if (new_fov < 1.0f || new_ar > sz.x_ || new_ar < (1.0f / float(sz.y_)))
        return;

    float ws_size_y = near_z * std::tan(radians(fov) / 2.0f);
    float ws_size_x = ar * -ws_size_y;

    fvec2 ws_sub_pos(ws_size_x * (2.0f * norm_sel_pos.x_ - 1.0f),
                     ws_size_y * (2.0f * norm_sel_pos.y_ - 1.0f));
    fvec2 ws_sub_sz(2.0f * norm_sz.x_ * ws_size_x, 2.0f * norm_sz.y_ * ws_size_y);

    fmat4 moved_proj = perspective_from(ws_sub_pos.x_,
                                        ws_sub_pos.x_ + ws_sub_sz.x_,
                                        ws_sub_pos.y_ + ws_sub_sz.y_,
                                        ws_sub_pos.y_,
                                        near_z,
                                        1000.0f);
    f.Define(moved_proj * cam_comp_->GetView());
    PODVector<Drawable *> res;
    Octree * octree = scene_->GetComponent<Octree>();
    FrustumOctreeQuery fq(res, f, DRAWABLE_GEOMETRY);
    octree->GetDrawables(fq);
    sel_rect_selection_.Clear();
    for (int i = 0; i < res.Size(); ++i)
    {
        Node * nd = res[i]->GetNode();
        StaticModel * sm = static_cast<StaticModel *>(res[i]);
        Editor_Selector * es = nd->GetComponent<Editor_Selector>();

        if (es == nullptr)
            continue;

        const BoundingBox & bb = sm->GetWorldBoundingBox();

        PhysicsWorld * phys = scene_->GetComponent<PhysicsWorld>();

        bool left_drag = (norm_sel_pos.x_ < selection_rect_.z_);

        bool res = false;
        auto fiter = cached_raycasts_.Find(nd);
        if (left_drag || f.IsInside(bb) == Intersection::INSIDE)
        {
            if (fiter == cached_raycasts_.End())
            {
                // Do a raycast to the center and each point of the objects bounding box
                PhysicsRaycastResult ray_result;
                fvec3 cam_pos = cam_comp_->GetNode()->GetWorldPosition();
                fvec3 direction = nd->GetWorldPosition() - cam_pos;
                direction.Normalize();
                Ray cast_ray(cam_pos, direction);
                // Get the closest object for selection
                phys->RaycastSingle(ray_result, cast_ray, 100.0f);
                if (ray_result.body_ != nullptr)
                    res = (ray_result.body_->GetNode() == nd);
                cached_raycasts_[nd] = res;
            }
            else
            {
                res = fiter->second_;
            }
        }

        if (res)
        {
            sel_rect_selection_.Insert(nd);
        }
    }
}

void Editor_Selection_Controller::handle_input_event(Urho3D::StringHash event_type,
                                                     Urho3D::VariantMap & event_data)
{
    StringHash name = event_data[InputTrigger::P_TRIGGER_NAME].GetStringHash();
    int state = event_data[InputTrigger::P_TRIGGER_STATE].GetInt();
    Vector2 norm_mpos = event_data[InputTrigger::P_VIEWPORT_NORM_MPOS].GetVector2();
    Vector2 norm_mdelta = event_data[InputTrigger::P_VIEWPORT_NORM_MDELTA].GetVector2();
    int wheel = event_data[InputTrigger::P_MOUSE_WHEEL].GetInt();
    int vp_ind = event_data[InputTrigger::P_VIEWPORT_INDEX].GetInt();

    if (!viewports_.Contains(vp_ind))
        return;
    
    Viewport * vp = GetSubsystem<Renderer>()->GetViewport(vp_ind);
    irect vp_sz = vp->GetRect();
    if (vp_sz.Max() == ivec2())
        vp_sz = vp->GetView()->GetViewRect();
    ui_root_->SetSize(vp_sz.Size());
    ui_root_->SetPosition(vp_sz.Min());

    if (name == hashes_[8])
    {
        toggle_occ_debug_selection();
    }
    if (name == hashes_[3])
    {
        // drag_point.w_ is a value used to detect if we are dragging - reset to 0 when draggin stops and set to 1 when
        // dragging starts
        if (drag_point_.w_ > 0.5f)
        {
            // Project the mouse movement vector to the camera projection plane that the collision occured
            // This is dependent on screen resolution - we have the normalized mdelta
            fvec3 cam_pos = cam_comp_->GetNode()->GetWorldPosition();
            drag_point_.w_ = 1.0f;
            fvec4 screen_space = cam_comp_->GetProjection() * cam_comp_->GetView() * drag_point_;
            screen_space /= screen_space.w_;
            screen_space.x_ += norm_mdelta.x_ * 2.0f;
            screen_space.y_ -= norm_mdelta.y_ * 2.0f;
            fvec4 new_posv4 =
                (cam_comp_->GetProjection() * cam_comp_->GetView()).Inverse() * screen_space;
            new_posv4 /= new_posv4.w_;

            fvec3 new_pos(new_posv4.x_, new_posv4.y_, new_posv4.z_);
            fvec3 drag_pnt(drag_point_.x_, drag_point_.y_, drag_point_.z_);
            fvec3 raw_translation = new_pos - drag_pnt;

            int state = movement_flag_;
            if (state < 7)
            {
                if (state == 0)
                    state = X_MOVE_FLAG | Y_MOVE_FLAG;

                fvec3 normal = cam_comp_->GetNode()->GetWorldDirection();
                normal *= fvec3(float((state & X_MOVE_FLAG) != X_MOVE_FLAG),
                                float((state & Y_MOVE_FLAG) != Y_MOVE_FLAG),
                                float((state & Z_MOVE_FLAG) != Z_MOVE_FLAG));
                normal.Normalize();
                fvec3 cast_vec = (new_pos - cam_pos);
                fvec3 proj_vec = raw_translation.ProjectOntoPlane(fvec3(0, 0, 0), normal);
                fvec3 norm_vec = proj_vec - raw_translation;

                raw_translation +=
                    (norm_vec.LengthSquared() / norm_vec.DotProduct(cast_vec)) * cast_vec;

                raw_translation *= fvec3(float((state & X_MOVE_FLAG) == X_MOVE_FLAG),
                                         float((state & Y_MOVE_FLAG) == Y_MOVE_FLAG),
                                         float((state & Z_MOVE_FLAG) == Z_MOVE_FLAG));
            }
            drag_point_ += fvec4(raw_translation, 1.0f);
            frame_translation_ += raw_translation;
        }

        if (ui_selection_rect_->IsVisible())
        {
            ivec2 sz = ui_root_->GetSize();
            fvec2 anchor_point(selection_rect_.z_, selection_rect_.w_);
            fvec2 new_sz = norm_mpos - anchor_point;

            // Set the selection rect position by default to the anchor point (point where first clicked on started the selection
            // rect), then if the rectangle formed by this point as the upper left corner and the new mouse pos is the lower left
            // corner makes a negative size (where down and to the right are positive), then we gotta set the selection rect position
            // at the new mouse pos and make the size = abs(size)
            selection_rect_.x_ = anchor_point.x_;
            selection_rect_.y_ = anchor_point.y_;
            if (new_sz.x_ < 0.0f)
                selection_rect_.x_ = norm_mpos.x_;
            if (new_sz.y_ < 0.0f)
                selection_rect_.y_ = norm_mpos.y_;

            fvec2 norm_sel_pos(selection_rect_.x_, selection_rect_.y_);
            // Convert set the selection rect position and size in pixels (multiply the norm coords by root element pixel size)
            ui_selection_rect_->SetSize(ivec2(Abs(new_sz.x_) * sz.x_, Abs(new_sz.y_) * sz.y_));
            ui_selection_rect_->SetPosition(
                ivec2(selection_rect_.x_ * sz.x_, selection_rect_.y_ * sz.y_));
            _add_to_selection_from_rect();
        }
    }
    else if (name == hashes_[4])
    {
        if (state == T_BEGIN)
        {
            // The Z and W components are the anchor point for the selection rectangle
            selection_rect_.x_ = selection_rect_.z_ = norm_mpos.x_;
            selection_rect_.y_ = selection_rect_.w_ = norm_mpos.y_;

            ivec2 sz = ui_root_->GetSize();
            ivec2 sz_c(norm_mpos.x_ * sz.x_, norm_mpos.y_ * sz.y_);
            ui_selection_rect_->SetPosition(sz_c);
            ui_selection_rect_->SetSize(ivec2(0, 0));
            ui_selection_rect_->SetVisible(true);
        }
        else
        {
            if (ui_selection_rect_->IsVisible())
            {
                // copy our selection and clear it
                auto sel_iter = sel_rect_selection_.Begin();
                while (sel_iter != sel_rect_selection_.End())
                {
                    selection_.Insert(*sel_iter);
                    ++sel_iter;
                }
                cached_raycasts_.Clear();
                ui_selection_rect_->SetVisible(false);
            }
        }
    }
    else if (name == hashes_[5])
    {
        if (state == T_BEGIN)
            movement_flag_ |= Z_MOVE_FLAG;
        else
            movement_flag_ &= ~Z_MOVE_FLAG;
    }
    else if (name == hashes_[6])
    {
        if (state == T_BEGIN)
            movement_flag_ |= X_MOVE_FLAG;
        else
            movement_flag_ &= ~X_MOVE_FLAG;
    }
    else if (name == hashes_[7])
    {
        if (state == T_BEGIN)
            movement_flag_ |= Y_MOVE_FLAG;
        else
            movement_flag_ &= ~Y_MOVE_FLAG;
    }
    else if (hashes_.Contains(name))
    {
        Octree * oct = scene_->GetComponent<Octree>();
        PODVector<RayQueryResult> res;
        // Get the closest object for selection
        RayOctreeQuery q(res, cam_comp_->GetScreenRay(norm_mpos.x_, norm_mpos.y_));
        oct->RaycastSingle(q);

        // Go through the results from the query - there really should only be one result since we are only doing
        // a single raycast
        for (int i = 0; i < res.Size(); ++i)
        {
            RayQueryResult & cr = res[i];

            Node * nd = cr.node_;

            StaticModelGroup * smg = nullptr;
            StaticModel * sm = nullptr;

            // If the object hit is a static model group then get the instance node
            if (cr.drawable_->IsInstanceOf<StaticModelGroup>())
                nd = static_cast<StaticModelGroup *>(cr.drawable_)->GetInstanceNode(cr.subObject_);

            Editor_Selector * es = cr.node_->GetComponent<Editor_Selector>();
            if (es != nullptr)
            {
                if (name == hashes_[0])
                {
                    if (!es->is_selected())
                    {
                        clear_selection();
                        selection_.Insert(cr.node_);
                    }
                }
                else if (name == hashes_[1])
                {
                    drag_point_ = fvec4();
                    if (state == T_BEGIN)
                        drag_point_ = fvec4(cr.position_, 1.0f);
                    else
                    {
                        if (ui_selection_rect_->IsVisible())
                        {
                            // copy our selection and clear it
                            auto sel_iter = sel_rect_selection_.Begin();
                            while (sel_iter != sel_rect_selection_.End())
                            {
                                selection_.Insert(*sel_iter);
                                ++sel_iter;
                            }

                            cached_raycasts_.Clear();
                            ui_selection_rect_->SetVisible(false);
                        }
                        if (!move_allowed_)
                        {
                            translate_selection(-total_drag_translation_);
                        }
                        total_drag_translation_ = fvec3();
                        do_snap_ = true;
                    }
                }
                else if (name == hashes_[2])
                {
                    if (es->is_selected())
                    {
                        remove_from_selection(nd);
                    }
                    else
                    {
                        selection_.Insert(cr.node_);
                    }
                }
            }
            else if (name == hashes_[0])
            {
                drag_point_ = fvec4();
                clear_selection();
            }
        }

        if (res.Empty())
        {
            if (name == hashes_[0])
            {
                drag_point_ = fvec4();
                clear_selection();
            }

            if (state == T_BEGIN)
            {
                // The Z and W components are the anchor point for the selection rectangle
                selection_rect_.x_ = selection_rect_.z_ = norm_mpos.x_;
                selection_rect_.y_ = selection_rect_.w_ = norm_mpos.y_;

                ivec2 sz = ui_root_->GetSize();
                ivec2 sz_c(norm_mpos.x_ * sz.x_, norm_mpos.y_ * sz.y_);
                ui_selection_rect_->SetPosition(sz_c);
                ui_selection_rect_->SetSize(ivec2(0, 0));
                ui_selection_rect_->SetVisible(true);
            }
            else
            {
                if (ui_selection_rect_->IsVisible())
                {
                    // copy our selection and clear it
                    auto sel_iter = sel_rect_selection_.Begin();
                    while (sel_iter != sel_rect_selection_.End())
                    {
                        selection_.Insert(*sel_iter);
                        ++sel_iter;
                    }

                    cached_raycasts_.Clear();
                    ui_selection_rect_->SetVisible(false);
                }
            }
        }
    }
}

void Editor_Selection_Controller::register_context(Urho3D::Context * ctxt)
{
    ctxt->RegisterFactory<Editor_Selection_Controller>();
}