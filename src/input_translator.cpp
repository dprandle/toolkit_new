#include <input_translator.h>
#include <mtdebug_print.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Input/Input.h>

#ifdef QT_BUILD
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <SDL/SDL.h>
#endif

using namespace Urho3D;

bool Input_Context::add_trigger(Input_Action_Trigger * trigger)
{
    // Make sure we have no absolute duplicates (not allowed due to stacking contexts)
    Trigger_Range tr = get_triggers(trigger->condition_);
    while (tr.first != tr.second)
    {
        if (*(tr.first->second) == *trigger)
            return false;
        ++tr.first;
    }

    t_map_.emplace(trigger->condition_.lookup_key_, trigger);
    return true;
}

Input_Action_Trigger * Input_Context::create_trigger(const Input_Action_Trigger & trig)
{
    Input_Action_Trigger * trigger = new Input_Action_Trigger(trig);
    if (add_trigger(trigger))
        return trigger;
    delete trigger;
    return nullptr;
}

bool Input_Context::destroy_trigger(Input_Action_Trigger * trigger)
{
    Input_Action_Trigger * trg = remove_trigger(trigger);

    if (trg != nullptr)
    {
        delete trg;
        return true;
    }
    return false;
}

bool Input_Context::destroy_triggers(const Trigger_Condition & cond)
{
    std::vector<Input_Action_Trigger *> triggs = remove_triggers(cond);
    for (int i = 0; i < triggs.size(); ++i)
        delete triggs[i];
    if (triggs.empty())
        return false;
    return true;
}

Input_Action_Trigger * Input_Context::remove_trigger(Input_Action_Trigger * trigger)
{
    auto iter = t_map_.begin();
    while (iter != t_map_.end())
    {
        if (iter->second == trigger)
        {
            t_map_.erase(iter);
            return trigger;
        }
        ++iter;
    }
    return nullptr;
}

std::vector<Input_Action_Trigger *> Input_Context::remove_triggers(const Trigger_Condition & cond)
{
    std::vector<Input_Action_Trigger *> ret;
    Trigger_Range tr = get_triggers(cond);
    while (tr.first != tr.second)
    {
        ret.push_back(tr.first->second);
        tr.first = t_map_.erase(tr.first);
    }
    return ret;
}

Trigger_Range Input_Context::get_triggers(const Trigger_Condition & cond)
{
    return t_map_.equal_range(cond.lookup_key_);
}

Input_Map::Input_Map()
{}

Input_Map::Input_Map(const Input_Map & copy_)
{}

Input_Map::~Input_Map()
{}

Input_Map & Input_Map::operator=(Input_Map rhs)
{
    return *this;
}

bool Input_Map::add_context(const Urho3D::StringHash & name, Input_Context * to_add)
{
    Input_Context * ctxt = get_context(name);
    if (ctxt == nullptr)
    {
        Pair<StringHash, Input_Context *> ins_pair;
        ins_pair.first_ = StringHash(name);
        ins_pair.second_ = to_add;
        contexts_.Insert(ins_pair);
        return true;
    }
    return false;
}

Input_Context * Input_Map::get_context(const Urho3D::StringHash & name)
{
    auto iter = contexts_.Find(name);
    if (iter != contexts_.End())
        return iter->second_;
    return nullptr;
}

Input_Context * Input_Map::create_context(const Urho3D::StringHash & name)
{
    Input_Context * ctxt = new Input_Context;
    if (!add_context(name, ctxt))
    {
        delete ctxt;
        return nullptr;
    }
    return ctxt;
}

bool Input_Map::destroy_context(const Urho3D::StringHash & name)
{
    Input_Context * ic = remove_context(name);
    if (ic != nullptr)
    {
        delete ic;
        return true;
    }
    return false;
}

bool Input_Map::destroy_context(Input_Context * to_destroy)
{
    to_destroy = remove_context(to_destroy);
    if (to_destroy != nullptr)
    {
        delete to_destroy;
        return true;
    }
    return false;
}

Input_Context * Input_Map::remove_context(const Urho3D::StringHash & name)
{
    auto iter = contexts_.Find(name);
    if (iter != contexts_.End())
    {
        Input_Context * ic = iter->second_;
        contexts_.Erase(iter);
        return ic;
    }
    return nullptr;
}

Input_Context * Input_Map::remove_context(Input_Context * to_remove)
{
    auto iter = contexts_.Begin();
    while (iter != contexts_.End())
    {
        if (iter->second_ == to_remove)
        {
            contexts_.Erase(iter);
            return to_remove;
        }
        ++iter;
    }
    return nullptr;
}

bool Input_Map::rename_context(const Urho3D::StringHash & old_name,
                               const Urho3D::StringHash & new_name)
{
    Input_Context * ic = remove_context(old_name);
    if (ic == nullptr)
        return false;
    return add_context(new_name, ic);
}

Input_Translator::Input_Translator(Urho3D::Context * context) : Object(context)
{}

Input_Translator::~Input_Translator()
{}

void Input_Translator::init()
{
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Input_Translator, handle_key_down));
    SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(Input_Translator, handle_key_up));
    SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(Input_Translator, handle_mouse_down));
    SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(Input_Translator, handle_mouse_up));
    SubscribeToEvent(E_MOUSEWHEEL, URHO3D_HANDLER(Input_Translator, handle_mouse_wheel));
    SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(Input_Translator, handle_mouse_move));
    GetSubsystem<Input>()->SetMouseVisible(true);
#ifdef QT_BUILD
    _init_key_map();
    _init_mouse_button_map();
#endif
}

void Input_Translator::release()
{
    UnsubscribeFromAllEvents();
}

void Input_Translator::handle_key_down(Urho3D::StringHash event_type,
                                       Urho3D::VariantMap & event_data)
{
    Trigger_Condition tc;
    tc.key_ = int32_t(event_data["Key"].GetInt());
    tc.mouse_button_ = 0;
    int32_t qualifiers(event_data["Qualifiers"].GetInt());
    int32_t mouse_buttons(event_data["Buttons"].GetInt());

    for (int i = context_stack_.Size(); i > 0; --i)
    {
        Input_Context * cur_ic = context_stack_[i - 1];
        Input_Action_Trigger * trig = nullptr;

        // Go through every trigger - if the current mb and quals are contained in the required qual and mb fields
        // and the allowed
        // First find all triggers with the exact match for key and mouse qualifiers
        Trigger_Range tr = cur_ic->get_triggers(tc);

        while (tr.first != tr.second)
        {
            Input_Action_Trigger * trig = tr.first->second;

            // Check the qualifier and mouse button required conditions
            bool pass_qual_required((trig->qual_required_ & qualifiers) == trig->qual_required_);
            bool pass_mb_required((trig->mb_required_ & mouse_buttons) == trig->mb_required_);

            // Check the qualifier and mouse button allowed conditions
            int32_t allowed_quals = trig->qual_required_ | trig->qual_allowed_;
            int32_t allowed_mb = trig->mb_required_ | trig->mb_allowed_;
            bool pass_qual_allowed(((allowed_quals & QUAL_ANY) == QUAL_ANY) ||
                                   (qualifiers | allowed_quals) == allowed_quals);
            bool pass_mb_allowed((mouse_buttons | allowed_mb) == allowed_mb);

            // If passes all the conditions, send the event for the trigger and mark trigger as active
            if (!_trigger_already_active(trig) && pass_qual_required && pass_mb_required &&
                pass_qual_allowed && pass_mb_allowed)
            {
                if ((trig->trigger_state_ & T_BEGIN) == T_BEGIN)
                {
                    using namespace InputTrigger;
                    VariantMap event_data;
                    event_data[P_TRIGGER_NAME] = trig->name_;
                    event_data[P_TRIGGER_STATE] = T_BEGIN;
                    event_data[P_NORM_MPOS] = current_norm_mpos_;
                    event_data[P_NORM_MDELTA] = Vector2();
                    event_data[P_MOUSE_WHEEL] = 0;

                    // Send an event to each viewport
                    Urho3D::Vector<Viewport_Info> vp_info_vec;
                    _fill_vp_info(vp_info_vec, current_norm_mpos_, fvec2());
                    for (int vpind = 0; vpind < vp_info_vec.Size(); ++vpind)
                    {
                        event_data[P_VIEWPORT_NORM_MPOS] = vp_info_vec[vpind].vp_norm_mpos;
                        event_data[P_VIEWPORT_NORM_MDELTA] = vp_info_vec[vpind].vp_norm_mdelta;
                        event_data[P_VIEWPORT_INDEX] = vp_info_vec[vpind].vp_index;
                        SendEvent(E_INPUT_TRIGGER, event_data);
                    }
                }
                active_triggers_.Push(trig);
            }

            ++tr.first;
        }
    }
}

bool Input_Translator::_trigger_already_active(Input_Action_Trigger * trig)
{
    auto iter = active_triggers_.Begin();
    while (iter != active_triggers_.End())
    {
        if (*trig == **iter)
            return true;
        ++iter;
    }
    return false;
}

void Input_Translator::handle_key_up(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data)
{
    Trigger_Condition tc;
    tc.key_ = int32_t(event_data["Key"].GetInt());
    tc.mouse_button_ = 0;
    int32_t mouse_buttons = int32_t(event_data["Buttons"].GetInt());
    int32_t qualifiers = int32_t(event_data["Qualifiers"].GetInt());

    auto iter = active_triggers_.Begin();
    while (iter != active_triggers_.End())
    {
        Input_Action_Trigger * curt = *iter;

        // Always end the action and send the trigger when the main key is depressed - dont care
        // about any of the qualifier situation
        if (tc.key_ == curt->condition_.key_)
        {
            if ((curt->trigger_state_ & T_END) == T_END)
            {
                VariantMap event_data;
                using namespace InputTrigger;
                event_data[P_TRIGGER_NAME] = curt->name_;
                event_data[P_TRIGGER_STATE] = T_END;
                event_data[P_NORM_MPOS] = current_norm_mpos_;
                event_data[P_NORM_MDELTA] = Vector2();
                event_data[P_MOUSE_WHEEL] = 0;

                // Send an event to each viewport
                Urho3D::Vector<Viewport_Info> vp_info_vec;
                _fill_vp_info(vp_info_vec, current_norm_mpos_, fvec2());
                for (int vpind = 0; vpind < vp_info_vec.Size(); ++vpind)
                {
                    event_data[P_VIEWPORT_NORM_MPOS] = vp_info_vec[vpind].vp_norm_mpos;
                    event_data[P_VIEWPORT_NORM_MDELTA] = vp_info_vec[vpind].vp_norm_mdelta;
                    event_data[P_VIEWPORT_INDEX] = vp_info_vec[vpind].vp_index;
                    SendEvent(E_INPUT_TRIGGER, event_data);
                }
            }
            iter = active_triggers_.Erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void Input_Translator::_normalize_mpos(Vector2 & to_norm)
{
    IntVector2 size = GetSubsystem<Graphics>()->GetSize();
    to_norm.x_ /= float(size.x_);
    to_norm.y_ /= float(size.y_);
}

void Input_Translator::_fill_vp_info(Urho3D::Vector<Viewport_Info> & vp_inf_vec,
                                     const fvec2 & norm_mpos,
                                     const fvec2 & norm_mdelta)
{
    ivec2 sz = GetSubsystem<Graphics>()->GetSize();
    fvec2 sz_f(sz.x_, sz.y_);

    Renderer * rend = GetSubsystem<Renderer>();

    // First figure out which viewport we are in
    for (int i = 0; i < rend->GetNumViewports(); ++i)
    {
        Viewport * vp = rend->GetViewport(i);
        irect scrn = vp->GetRect();
        fvec2 ll(scrn.left_, scrn.top_);
        fvec2 ur(scrn.right_, scrn.bottom_);
        fvec2 mpos = norm_mpos * sz_f;
        fvec2 mdelta = norm_mdelta * sz_f;

        if (scrn == irect())
            ur = sz_f;

        frect fr(ll, ur);
        if (fr.IsInside(mpos) == Intersection::INSIDE)
        {
            Viewport_Info vinf;
            vinf.vp_index = i;
            vinf.vp_norm_mpos = (mpos - ll) / fr.Size();
            vinf.vp_norm_mdelta = mdelta / fr.Size();
            vp_inf_vec.Push(vinf);
        }
    }
}

void Input_Translator::handle_mouse_down(Urho3D::StringHash event_type,
                                         Urho3D::VariantMap & event_data)
{
    Trigger_Condition tc;
    tc.key_ = 0;
    tc.mouse_button_ = event_data["Button"].GetInt();
    int32_t mouse_buttons = int32_t(event_data["Buttons"].GetInt());
    int32_t qualifiers = int32_t(event_data["Qualifiers"].GetInt());
    int32_t wheel = 0;
    Vector2 norm_mdelta;

    if (tc.mouse_button_ == MOUSEB_WHEEL)
        wheel = int32_t(event_data["Wheel"].GetInt());

    if (tc.mouse_button_ == MOUSEB_MOVE)
    {
        Vector2 current_mpos(float(event_data["X"].GetInt()), float(event_data["Y"].GetInt()));
        _normalize_mpos(current_mpos);
        norm_mdelta = current_mpos - current_norm_mpos_;
        current_norm_mpos_ = current_mpos;
    }

    for (int i = context_stack_.Size(); i > 0; --i)
    {
        Input_Context * cur_ic = context_stack_[i - 1];
        Input_Action_Trigger * trig = nullptr;

        // Go through every trigger - if the current mb and quals are contained in the required qual and mb fields
        // and the allowed
        // First find all triggers with the exact match for key and mouse qualifiers
        Trigger_Range tr = cur_ic->get_triggers(tc);
        while (tr.first != tr.second)
        {
            Input_Action_Trigger * trig = tr.first->second;

            // Check the qualifier and mouse button required conditions
            bool pass_qual_required((trig->qual_required_ & qualifiers) == trig->qual_required_);
            bool pass_mb_required((trig->mb_required_ & mouse_buttons) == trig->mb_required_);

            // Check the qualifier and mouse button allowed conditions
            int32_t allowed_quals = trig->qual_required_ | trig->qual_allowed_;
            int32_t allowed_mb =
                trig->mb_required_ | trig->mb_allowed_ | trig->condition_.mouse_button_;
            bool pass_qual_allowed(((allowed_quals & QUAL_ANY) == QUAL_ANY) ||
                                   (qualifiers | allowed_quals) == allowed_quals);
            bool pass_mb_allowed((mouse_buttons | allowed_mb) == allowed_mb);

            // If passes all the conditions, send the event for the trigger and mark trigger as active

            if (!_trigger_already_active(trig) && pass_qual_required && pass_mb_required &&
                pass_qual_allowed && pass_mb_allowed)
            {
                if ((trig->trigger_state_ & T_BEGIN) == T_BEGIN)
                {
                    VariantMap event_data;
                    using namespace InputTrigger;
                    event_data[P_TRIGGER_NAME] = trig->name_;
                    event_data[P_TRIGGER_STATE] = T_BEGIN;
                    event_data[P_NORM_MPOS] = current_norm_mpos_;
                    event_data[P_NORM_MDELTA] = norm_mdelta;
                    event_data[P_MOUSE_WHEEL] = wheel;

                    // Send an event to each viewport
                    Urho3D::Vector<Viewport_Info> vp_info_vec;
                    _fill_vp_info(vp_info_vec, current_norm_mpos_, norm_mdelta);
                    for (int vpind = 0; vpind < vp_info_vec.Size(); ++vpind)
                    {
                        event_data[P_VIEWPORT_NORM_MPOS] = vp_info_vec[vpind].vp_norm_mpos;
                        event_data[P_VIEWPORT_NORM_MDELTA] = vp_info_vec[vpind].vp_norm_mdelta;
                        event_data[P_VIEWPORT_INDEX] = vp_info_vec[vpind].vp_index;
                        SendEvent(E_INPUT_TRIGGER, event_data);
                    }
                }

                if (tc.mouse_button_ != MOUSEB_WHEEL && tc.mouse_button_ != MOUSEB_MOVE)
                    active_triggers_.Push(trig);
            }

            ++tr.first;
        }
    }
}

void Input_Translator::handle_mouse_up(Urho3D::StringHash event_type,
                                       Urho3D::VariantMap & event_data)
{
    Trigger_Condition tc;
    tc.key_ = 0;
    tc.mouse_button_ = event_data["Button"].GetInt();
    int32_t mouse_buttons = int32_t(event_data["Buttons"].GetInt());
    int32_t qualifiers = int32_t(event_data["Qualifiers"].GetInt());

    auto iter = active_triggers_.Begin();
    while (iter != active_triggers_.End())
    {
        Input_Action_Trigger * curt = *iter;

        if (tc.mouse_button_ == curt->condition_.mouse_button_)
        {
            if (((curt->trigger_state_ & T_END) == T_END))
            {
                VariantMap event_data;
                using namespace InputTrigger;
                event_data[P_TRIGGER_NAME] = curt->name_;
                event_data[P_TRIGGER_STATE] = T_END;
                event_data[P_NORM_MPOS] = current_norm_mpos_;
                event_data[P_NORM_MDELTA] = Vector2();
                event_data[P_MOUSE_WHEEL] = 0;

                // Send an event to each viewport
                Urho3D::Vector<Viewport_Info> vp_info_vec;
                _fill_vp_info(vp_info_vec, current_norm_mpos_, fvec2());
                for (int vpind = 0; vpind < vp_info_vec.Size(); ++vpind)
                {
                    event_data[P_VIEWPORT_NORM_MPOS] = vp_info_vec[vpind].vp_norm_mpos;
                    event_data[P_VIEWPORT_NORM_MDELTA] = vp_info_vec[vpind].vp_norm_mdelta;
                    event_data[P_VIEWPORT_INDEX] = vp_info_vec[vpind].vp_index;
                    SendEvent(E_INPUT_TRIGGER, event_data);
                }
            }
            iter = active_triggers_.Erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void Input_Translator::handle_mouse_wheel(Urho3D::StringHash event_type,
                                          Urho3D::VariantMap & event_data)
{
    event_data["Button"] = MOUSEB_WHEEL;
    handle_mouse_down(event_type, event_data);
}

void Input_Translator::handle_mouse_move(Urho3D::StringHash event_type,
                                         Urho3D::VariantMap & event_data)
{
    event_data["Button"] = MOUSEB_MOVE;
    handle_mouse_down(event_type, event_data);
}

void Input_Translator::push_context(Input_Context * ctxt)
{
    context_stack_.Push(ctxt);
}

void Input_Translator::pop_context()
{
    context_stack_.Pop();
}

//#ifdef QT_BUILD
void Input_Translator::qt_key_press(QKeyEvent * e)
{
    SDL_Event sdl_event;
    if (e->isAutoRepeat())
        return;
    sdl_event.type = SDL_KEYDOWN;
    sdl_event.key.keysym.sym = _convert_Qtkey_to_SDL(e->key());
    sdl_event.key.keysym.mod = _convert_Qtkey_modifier_to_SDL(e->modifiers());
    sdl_event.key.timestamp = e->timestamp();
    sdl_event.key.repeat = int(e->isAutoRepeat());
    sdl_event.key.state = SDL_PRESSED;
    SDL_PushEvent(&sdl_event);
}

void Input_Translator::qt_key_release(QKeyEvent * e)
{
    SDL_Event sdl_event;
    if (e->isAutoRepeat())
        return;
    sdl_event.type = SDL_KEYUP;
    sdl_event.key.keysym.sym = _convert_Qtkey_to_SDL(e->key());
    sdl_event.key.keysym.mod = _convert_Qtkey_modifier_to_SDL(e->modifiers());
    sdl_event.key.timestamp = e->timestamp();
    sdl_event.key.state = SDL_RELEASED;
    sdl_event.key.repeat = int(e->isAutoRepeat());
    SDL_PushEvent(&sdl_event);
}

void Input_Translator::qt_mouse_wheel(QWheelEvent * e)
{
    SDL_Event sdl_event;
    sdl_event.type = SDL_MOUSEWHEEL;
    sdl_event.wheel.x = e->angleDelta().x() / 8;
    sdl_event.wheel.y = e->angleDelta().y() / 8;
    sdl_event.wheel.timestamp = e->timestamp();
    sdl_event.wheel.direction = SDL_MOUSEWHEEL_NORMAL;
    SDL_PushEvent(&sdl_event);
}

void Input_Translator::qt_mouse_move(QMouseEvent * e)
{
    SDL_Event sdl_event;
    sdl_event.type = SDL_MOUSEMOTION;
    sdl_event.motion.timestamp = e->timestamp();
    sdl_event.motion.x = e->pos().x();
    sdl_event.motion.y = e->pos().y();
    sdl_event.motion.state = _convert_Qtmb_modifier_to_SDL(e->buttons());
    SDL_PushEvent(&sdl_event);
}

void Input_Translator::qt_mouse_press(QMouseEvent * e)
{
    SDL_Event sdl_event;
    sdl_event.type = SDL_MOUSEBUTTONDOWN;
    sdl_event.button.button = _convert_Qtmb_to_SDL(e->button());
    sdl_event.button.clicks = 1;
    sdl_event.button.x = e->pos().x();
    sdl_event.button.y = e->pos().y();
    sdl_event.button.timestamp = e->timestamp();
    sdl_event.button.state = SDL_PRESSED;
    SDL_PushEvent(&sdl_event);
}

void Input_Translator::qt_mouse_release(QMouseEvent * e)
{
    SDL_Event sdl_event;
    sdl_event.type = SDL_MOUSEBUTTONUP;
    sdl_event.button.button = _convert_Qtmb_to_SDL(e->button());
    sdl_event.button.clicks = 1;
    sdl_event.button.x = e->pos().x();
    sdl_event.button.y = e->pos().y();
    sdl_event.button.timestamp = e->timestamp();
    sdl_event.button.state = SDL_RELEASED;
    SDL_PushEvent(&sdl_event);
}

void Input_Translator::qt_window_resize(QResizeEvent * e)
{
    if (GetSubsystem<Engine>()->IsInitialized())
    {
        int width = e->size().width();
        int height = e->size().height();

        Graphics * graphics = GetSubsystem<Graphics>();

        SDL_Window * win = (SDL_Window *)graphics->GetWindow();
        SDL_SetWindowSize(win, width, height);
    }
}

void Input_Translator::_init_key_map()
{
    keymap_[Qt::Key_unknown] = SDLK_UNKNOWN;
    keymap_[Qt::Key_Escape] = SDLK_ESCAPE;
    keymap_[Qt::Key_Tab] = SDLK_TAB;
    keymap_[Qt::Key_Backspace] = SDLK_BACKSPACE;
    keymap_[Qt::Key_Return] = SDLK_RETURN;
    keymap_[Qt::Key_Enter] = SDLK_KP_ENTER;
    keymap_[Qt::Key_Insert] = SDLK_INSERT;
    keymap_[Qt::Key_Delete] = SDLK_DELETE;
    keymap_[Qt::Key_Pause] = SDLK_PAUSE;
    keymap_[Qt::Key_Print] = SDLK_PRINTSCREEN;
    keymap_[Qt::Key_SysReq] = SDLK_SYSREQ;
    keymap_[Qt::Key_Home] = SDLK_HOME;
    keymap_[Qt::Key_End] = SDLK_END;
    keymap_[Qt::Key_Left] = SDLK_LEFT;
    keymap_[Qt::Key_Right] = SDLK_RIGHT;
    keymap_[Qt::Key_Up] = SDLK_UP;
    keymap_[Qt::Key_Down] = SDLK_DOWN;
    keymap_[Qt::Key_PageUp] = SDLK_PAGEUP;
    keymap_[Qt::Key_PageDown] = SDLK_PAGEDOWN;
    keymap_[Qt::Key_Shift] = SDLK_LSHIFT;
    keymap_[Qt::Key_Control] = SDLK_LCTRL;
    keymap_[Qt::Key_Alt] = SDLK_LALT;
    keymap_[Qt::Key_CapsLock] = SDLK_CAPSLOCK;
    keymap_[Qt::Key_NumLock] = SDLK_NUMLOCKCLEAR;
    keymap_[Qt::Key_ScrollLock] = SDLK_SCROLLLOCK;
    keymap_[Qt::Key_F1] = SDLK_F1;
    keymap_[Qt::Key_F2] = SDLK_F2;
    keymap_[Qt::Key_F3] = SDLK_F3;
    keymap_[Qt::Key_F4] = SDLK_F4;
    keymap_[Qt::Key_F5] = SDLK_F5;
    keymap_[Qt::Key_F6] = SDLK_F6;
    keymap_[Qt::Key_F7] = SDLK_F7;
    keymap_[Qt::Key_F8] = SDLK_F8;
    keymap_[Qt::Key_F9] = SDLK_F9;
    keymap_[Qt::Key_F10] = SDLK_F10;
    keymap_[Qt::Key_F11] = SDLK_F11;
    keymap_[Qt::Key_F12] = SDLK_F12;
    keymap_[Qt::Key_F13] = SDLK_F13;
    keymap_[Qt::Key_F14] = SDLK_F14;
    keymap_[Qt::Key_F15] = SDLK_F15;
    keymap_[Qt::Key_Menu] = SDLK_MENU;
    keymap_[Qt::Key_Help] = SDLK_HELP;

    for (int key = 'A'; key <= 'Z'; key++)
        keymap_[Qt::Key(key)] = key + 32;

    for (int key = '0'; key <= '9'; key++)
        keymap_[Qt::Key(key)] = key;
}

void Input_Translator::_init_mouse_button_map()
{
    mouse_button_map_[Qt::LeftButton] = SDL_BUTTON_LEFT;
    mouse_button_map_[Qt::RightButton] = SDL_BUTTON_RIGHT;
    mouse_button_map_[Qt::MiddleButton] = SDL_BUTTON_MIDDLE;
    mouse_button_map_[Qt::XButton1] = SDL_BUTTON_X1;
    mouse_button_map_[Qt::XButton2] = SDL_BUTTON_X2;
}

int Input_Translator::_convert_Qtkey_to_SDL(int qtKey)
{
    auto iter = keymap_.Find(qtKey);
    if (iter != keymap_.End())
        return iter->second_;
    return 0;
}

int Input_Translator::_convert_Qtkey_modifier_to_SDL(Qt::KeyboardModifiers qtKeyModifiers)
{
    int sdlModifiers = KMOD_NONE;
    int64_t flg = qtKeyModifiers;
    if ((flg & Qt::ShiftModifier) == Qt::ShiftModifier)
    {
        sdlModifiers |= KMOD_SHIFT;
    }
    if ((flg & Qt::ControlModifier) == Qt::ControlModifier)
    {
        sdlModifiers |= KMOD_CTRL;
    }
    if ((flg & Qt::AltModifier) == Qt::AltModifier)
    {
        sdlModifiers |= KMOD_ALT;
    }

    return sdlModifiers;
}

int Input_Translator::_convert_Qtmb_modifier_to_SDL(Qt::MouseButtons buttons)
{
    int mods = 0;
    if ((buttons & Qt::LeftButton) == Qt::LeftButton)
        mods |= SDL_BUTTON_LMASK;
    if ((buttons & Qt::RightButton) == Qt::RightButton)
        mods |= SDL_BUTTON_RMASK;
    if ((buttons & Qt::MiddleButton) == Qt::MiddleButton)
        mods |= SDL_BUTTON_MMASK;
    if ((buttons & Qt::XButton1) == Qt::XButton1)
        mods |= SDL_BUTTON_X1MASK;
    if ((buttons & Qt::XButton2) == Qt::XButton2)
        mods |= SDL_BUTTON_X2MASK;

    return mods;
}

int Input_Translator::_convert_Qtmb_to_SDL(int qtbutton)
{
    auto iter = mouse_button_map_.Find(qtbutton);
    if (iter != mouse_button_map_.End())
        return iter->second_;
    return 0;
}
//#endif