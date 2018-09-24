#ifndef EDITOR_SELECTION
#define EDITOR_SELECTION

#include <Urho3D/Core/Object.h>
#include <set>

namespace Urho3D
{
class Scene;
class Node;
}

class input_context;

class EditorSelection: public Urho3D::Object
{
    URHO3D_OBJECT(EditorSelection, Urho3D::Object)
	
	public:

	EditorSelection(Urho3D::Context * context);
	~EditorSelection();

	void update(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	
	void handle_input_event(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	void setup_input_context(input_context * ctxt);

	void set_scene(Urho3D::Scene * scn_);

	void set_camera(Urho3D::Node * node_);
	
  private:

	std::set<Urho3D::Node*> selection;
	Urho3D::Scene * scn;
	Urho3D::Node * cam_node;
	
};

#endif
