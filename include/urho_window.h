#pragma once

#include <Urho3D/Core/Object.h>
#include <math_utils.h>

namespace Urho3D
{
class Engine;
}

class Input_Translator;
class Input_Map;

struct Urho_Window : public Urho3D::Object
{
    Urho_Window(Urho3D::Context * context, void * window_id);
    virtual ~Urho_Window();

    void update(bool focus);

    void make_current();

    void update_gl_context();

    Urho3D::Engine * engine_;

    Input_Translator * input_translator_;

    Input_Map * input_map_;

    void * gl_ctxt_;
};
