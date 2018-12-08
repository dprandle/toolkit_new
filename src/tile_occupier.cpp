#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <hex_tile_grid.h>
#include <mtdebug_print.h>
#include <tile_occupier.h>

using namespace Urho3D;

Tile_Occupier::Tile_Occupier(Urho3D::Context * context) : Component(context), draw_debug_(false)
{
    URHO3D_ACCESSOR_ATTRIBUTE(
        "Spaces", get_spaces, set_spaces, VariantVector, Variant::emptyVariantVector, AM_FILE);
    spaces_.Push(ivec3());
}

Tile_Occupier::~Tile_Occupier()
{}

void Tile_Occupier::build_from_model(Urho3D::Model * model)
{}

void Tile_Occupier::add(const ivec3 & grid)
{
    if (!spaces_.Contains(grid))
        spaces_.Push(grid);
}

void Tile_Occupier::remove(const ivec3 & grid)
{
    spaces_.Remove(grid);
    if (spaces_.Empty())
        spaces_.Push(ivec3());
}

const Urho3D::Vector<ivec3> & Tile_Occupier::tile_spaces()
{
    return spaces_;
}

void Tile_Occupier::register_context(Urho3D::Context * ctxt)
{
    ctxt->RegisterFactory<Tile_Occupier>();
}

void Tile_Occupier::enable_debug(bool enable)
{
    draw_debug_ = enable;
}

bool Tile_Occupier::debug_enabled()
{
    return draw_debug_;
}

void Tile_Occupier::set_spaces(const Urho3D::VariantVector & spaces)
{
    spaces_.Resize(spaces.Size());
    for (int i = 0; i < spaces.Size(); ++i)
        spaces_[i] = spaces[i].GetIntVector3();
}

Urho3D::VariantVector Tile_Occupier::get_spaces() const
{
    VariantVector spaces;
    spaces.Resize(spaces_.Size());
    for (int i = 0; i < spaces.Size(); ++i)
        spaces[i] = spaces_[i];
    return spaces;
}

void Tile_Occupier::OnNodeSet(Urho3D::Node * node)
{
    Component::OnNodeSet(node);
}

void Tile_Occupier::OnMarkedDirty(Urho3D::Node * node)
{
    Scene * scn = GetScene();
    Hex_Tile_Grid * tg = scn->GetComponent<Hex_Tile_Grid>();
    if (tg == nullptr)
        return;
    uint32_t node_id = node->GetID();
    tg->remove(spaces_, old_position_, Hex_Tile_Grid::Tile_Item(node_id));
    fvec3 new_position = node->GetPosition();
    tg->add(Hex_Tile_Grid::Tile_Item(node_id), spaces_, new_position);
    old_position_ = node->GetPosition();
}

void Tile_Occupier::DrawDebugGeometry(bool depth)
{
    Scene * scn = GetScene();
    if (scn == nullptr)
        return;

    DebugRenderer * deb = scn->GetComponent<DebugRenderer>();
    if (deb == nullptr)
        return;

    for (int i = 0; i < spaces_.Size(); ++i)
    {
        deb->AddCross(Hex_Tile_Grid::grid_to_world(spaces_[i], GetNode()->GetPosition()),
                      1.0f,
                      Color(0.0f, 0.0f, 1.0f),
                      depth);
    }
}