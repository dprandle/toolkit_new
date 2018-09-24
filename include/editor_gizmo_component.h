#ifndef EDITOR_GIZMO_COMP_H
#define EDITOR_GIZMO_COMP_H

#include <Urho3D/Scene/Component.h>

class EditorGizmoComponent : public Urho3D::Component
{

	URHO3D_OBJECT(EditorGizmoComponent, Urho3D::Object)

	public:

	EditorGizmoComponent(Urho3D::Context * context);
	~EditorGizmoComponent();

	void setup();
	
	private:

	
};

#endif
