#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Graphics.h>

#include <urho_window.h>
#include <input_translator.h>
#include <SDL/SDL.h>

using namespace Urho3D;

Urho_Window::Urho_Window(Context * context, void * window_id)
    : Object(context),
      engine_(new Engine(context)),
      input_translator_(new Input_Translator(context)),
      input_map_(new Input_Map())
{
    gl_ctxt_ = SDL_GL_GetCurrentContext();
    context_->RegisterSubsystem(input_translator_);
}

Urho_Window::~Urho_Window()
{
    input_translator_->release();
    context_->RemoveSubsystem<Input_Translator>();
    delete input_map_;
}

void Urho_Window::update(bool focus)
{
    make_current();
    // if (focus)
    // {
        if (!engine_->IsExiting())
            engine_->RunFrame();
    // }
    // else
    // {
    //     if (!engine_->IsExiting())
    //         engine_->Render();
    // }
}

void Urho_Window::make_current()
{
    Graphics * gfx = GetSubsystem<Graphics>();
    if (gfx != nullptr)
        SDL_GL_MakeCurrent(gfx->GetWindow(), gl_ctxt_);
}

void Urho_Window::update_gl_context()
{
    gl_ctxt_ = SDL_GL_GetCurrentContext();
}
