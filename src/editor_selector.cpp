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
    : Urho3D::StaticModel(context)
{}

void Editor_Selector::set_selected(bool select)
{
    Urho3D::Scene * scn = GetScene();
    if (scn == nullptr)
        return;

    Urho3D::StaticModel * comp = node_->GetComponent<StaticModel>();
    if (comp == nullptr)
        return;

    if ((select && is_selected()) || (!select && !is_selected()))
        return;

    comp->SetEnabled(!select);
    SetEnabled(select);
}

bool Editor_Selector::is_selected()
{
    return IsEnabled();
}

void Editor_Selector::toggle_selected()
{
    set_selected(!is_selected());
}

void Editor_Selector::register_context(Urho3D::Context * context)
{
    context->RegisterFactory<Editor_Selector>();
    URHO3D_ACCESSOR_ATTRIBUTE("Is Enabled", IsEnabled, SetEnabled, bool, true, Urho3D::AM_DEFAULT);
}