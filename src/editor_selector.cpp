#include <editor_selector.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/StaticModelGroup.h>
#include <Urho3D/IO/Log.h>
#include <string>

Editor_Selector::Editor_Selector(Urho3D::Context * context)
    : Component(context), id_of_component_to_control_(-1), sel_render_comp_(nullptr)
{}

void Editor_Selector::set_selected(Urho3D::Node * node, bool select)
{
    bool cur_selected = is_selected(node);
    if (select == cur_selected)
        return;

    Scene * scn = GetScene();
    if (scn == nullptr)
        return;
    StaticModel * comp = static_cast<StaticModel *>(scn->GetComponent(id_of_component_to_control_));
    if (comp == nullptr)
        return;

    if ((select && is_selected(node)) || (!select && !is_selected(node)))
        return;

    StaticModelGroup * ogp = nullptr;
    StaticModelGroup * gp = static_cast<StaticModelGroup *>(comp);
    if (sel_render_comp_->IsInstanceOf<StaticModelGroup>())
        ogp = static_cast<StaticModelGroup *>(sel_render_comp_);

    //URHO3D_LOGINFO("The selection node is " + String(uint64_t(node_)) + " and the passed in node is " + String(uint64_t(node)));
    // If node passed in is the same node that this component is attached to, it means either the node is a StaticModel
    // and we should disable our static model and enable the normal static model, or it means we are a static model group
    // and we should remove all instance nodes and add them back to the normal static model
    if (node == node_)
    {
        if (ogp == nullptr)
        {
            comp->SetEnabled(!select);
            sel_render_comp_->SetEnabled(select);
        }
        else if (!select)
        {
            int cnt = ogp->GetNumInstanceNodes();
            for (int i = 0; i < cnt; ++i)
            {
                Node * nd = ogp->GetInstanceNode(i);
                gp->AddInstanceNode(nd);
            }
            ogp->RemoveAllInstanceNodes();
        }
    }
    else if (ogp != nullptr)
    {
        if (select)
        {
            // Remove the instance node from the normal model and add it to the selection model
            gp->RemoveInstanceNode(node);
            ogp->AddInstanceNode(node);
        }
        else
        {
            ogp->RemoveInstanceNode(node);
            gp->AddInstanceNode(node);
        }
    }
}

bool Editor_Selector::is_selected(Urho3D::Node * node)
{
    // If node is our node - it means we are not a static model group and so do not need to worry
    // about sub objects
    if (node == node_)
    {
        return sel_render_comp_->IsEnabled();
    }
    else
    {
        // The node passed in is not the same node our static model is attached to (or static model group)
        // This means either the node is a sub object (if it is selected) or not if it is not selected
        StaticModelGroup * mg = static_cast<StaticModelGroup *>(sel_render_comp_);
        int cnt = mg->GetNumInstanceNodes();
        for (int i = 0; i < cnt; ++i)
        {
            Node * nd = mg->GetInstanceNode(i);
            if (mg->GetInstanceNode(i) == node)
                return true;
        }
        return false;
    }
}

void Editor_Selector::toggle_selected(Urho3D::Node * node)
{
    set_selected(node, !is_selected(node));
}

void Editor_Selector::set_render_component_to_control(int comp_id)
{
    id_of_component_to_control_ = comp_id;

    if (sel_render_comp_ != nullptr)
        node_->RemoveComponent(sel_render_comp_);

    if (id_of_component_to_control_ == -1)
        sel_render_comp_ = nullptr;
    else
    {
        Component * cmp = GetScene()->GetComponent(comp_id);
        StaticModel * smcmp = static_cast<StaticModel *>(cmp);
        ResourceCache * cache = GetSubsystem<ResourceCache>();
        Material * mat = cache->GetResource<Material>(selected_mat_);

        sel_render_comp_ = static_cast<StaticModel *>(node_->CreateComponent(cmp->GetType()));
        sel_render_comp_->SetModel(smcmp->GetModel());

        sel_render_comp_->SetMaterial(mat);
    }
}

void Editor_Selector::set_selection_material(const Urho3D::String & name)
{
    selected_mat_ = name;
    if (sel_render_comp_ != nullptr)
    {
        ResourceCache * cache = GetSubsystem<ResourceCache>();
        Material * mat = cache->GetResource<Material>(selected_mat_);
        sel_render_comp_->SetMaterial(mat);
    }
}

const Urho3D::String & Editor_Selector::selection_material()
{
    return selected_mat_;
}

void Editor_Selector::register_context(Urho3D::Context * context)
{
    context->RegisterFactory<Editor_Selector>();
}

void Editor_Selector::OnNodeSet(Urho3D::Node * node)
{
    Component * comp = node->GetComponent<StaticModelGroup>();
    if (comp == nullptr)
    {
        comp = node->GetComponent<StaticModel>();
        if (comp != nullptr)
            set_render_component_to_control(comp->GetID());
    }
    Component::OnNodeSet(node);
}
