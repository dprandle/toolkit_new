#include <input_translator.h>
#include <toolkit.h>
#include <scene_view.h>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>

#include <SDL/SDL.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Input/Input.h>

using namespace Urho3D;

bool input_context::add_trigger(
	input_action_trigger * trigger
	)
{
	// Make sure we have no absolute duplicates (not allowed due to stacking contexts)
	trigger_range tr = get_triggers(trigger->condition);
	while (tr.first != tr.second)
	{
		if (*(tr.first->second) == *trigger)
			return false;
		++tr.first;
	}

	t_map.emplace(trigger->condition.lookup_key,trigger);
	return true;
}

input_action_trigger * input_context::create_trigger(
	const trigger_condition & cond
	)
{
	input_action_trigger * trigger = new input_action_trigger;
	trigger->condition = cond;
	if (add_trigger(trigger))
		return trigger;
	delete trigger;
	return nullptr;
}

bool input_context::destroy_trigger(
	input_action_trigger * trigger
	)
{
	input_action_trigger * trg = remove_trigger(trigger);
	if (trg != nullptr)
	{
		delete trg;
		return true;
	}
	return false;	
}

bool input_context::destroy_triggers(
	const trigger_condition & cond
	)
{
	std::vector<input_action_trigger*> triggs = remove_triggers(cond);
	for (int i = 0; i < triggs.size(); ++i)
		delete triggs[i];
	if (triggs.empty())
		return false;
	return true;
}

input_action_trigger * input_context::remove_trigger(
	input_action_trigger * trigger
	)
{
	auto iter = t_map.begin();
	while (iter != t_map.end())
	{
		if (iter->second == trigger)
		{
			t_map.erase(iter);
			return trigger;
		}
		++iter;
	}
	return nullptr;
}

std::vector<input_action_trigger*> input_context::remove_triggers(
	const trigger_condition & cond
	)
{
	std::vector<input_action_trigger*> ret;
	trigger_range tr = get_triggers(cond);
	while (tr.first != tr.second)
	{
		ret.push_back(tr.first->second);
		tr.first = t_map.erase(tr.first);
	}
	return ret;
}
	
trigger_range input_context::get_triggers(
	const trigger_condition & cond
	)
{
	return t_map.equal_range(cond.lookup_key);
}
	
InputMap::InputMap()
{
	
}

InputMap::InputMap(const InputMap & copy_)
{
	
}

InputMap::~InputMap()
{
	
}

InputMap & InputMap::operator=(InputMap rhs)
{
	return *this;
}

bool InputMap::add_context(const Urho3D::StringHash & name, input_context * to_add)
{
	input_context * ctxt = get_context(name);
	if (ctxt == nullptr)
	{
		Pair<StringHash, input_context*> ins_pair;
		ins_pair.first_ = StringHash(name);
		ins_pair.second_ = ctxt;
		m_contexts.Insert(ins_pair);
		return true;
	}
	return false;
}

input_context * InputMap::get_context(const Urho3D::StringHash & name)
{
	auto iter = m_contexts.Find(name);
	if (iter != m_contexts.End())
		return iter->second_;
	return nullptr;
}
	
input_context * InputMap::create_context(const Urho3D::StringHash & name)
{
	input_context * ctxt = new input_context;
	if (!add_context(name, ctxt))
	{
		delete ctxt;
		return nullptr;
	}
	return ctxt;
}

bool InputMap::destroy_context(const Urho3D::StringHash & name)
{
	input_context * ic = remove_context(name);
	if (ic != nullptr)
	{
		delete ic;
		return true;
	}
	return false;
}

bool InputMap::destroy_context(input_context * to_destroy)
{
	to_destroy = remove_context(to_destroy);
	if (to_destroy != nullptr)
	{
		delete to_destroy;
		return true;
	}
	return false;
}

input_context * InputMap::remove_context(const Urho3D::StringHash & name)
{
	auto iter = m_contexts.Find(name);
	if (iter != m_contexts.End())
	{
		input_context * ic = iter->second_;
		m_contexts.Erase(iter);
		return ic;
	}
	return nullptr;
}

input_context * InputMap::remove_context(input_context * to_remove)
{
	auto iter = m_contexts.Begin();
	while (iter != m_contexts.End())
	{
		if (iter->second_ == to_remove)
		{
			m_contexts.Erase(iter);
			return to_remove;
		}
		++iter;
	}
	return nullptr;
}

bool InputMap::rename_context(const Urho3D::StringHash & old_name, const Urho3D::StringHash & new_name)
{
	input_context * ic = remove_context(old_name);
	if (ic == nullptr)
		return false;
	return add_context(new_name, ic);
}

InputTranslator::InputTranslator(Urho3D::Context * context):
	Object(context)
{
	context->RegisterSubsystem(this);
	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(InputTranslator, handle_key_down));
	SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(InputTranslator, handle_key_up));
	SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(InputTranslator, handle_mouse_down));
	SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(InputTranslator, handle_mouse_up));
	SubscribeToEvent(E_MOUSEWHEEL, URHO3D_HANDLER(InputTranslator, handle_mouse_wheel));
	SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(InputTranslator, handle_mouse_move));
	_init_key_map();
	_init_mouse_button_map();
	GetSubsystem<Input>()->SetMouseVisible(true);
}

InputTranslator::~InputTranslator()
{}

void InputTranslator::qt_key_press(QKeyEvent * e)
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

void InputTranslator::qt_key_release(QKeyEvent * e)
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

void InputTranslator::qt_mouse_wheel(QWheelEvent * e)
{
	SDL_Event sdl_event;
	sdl_event.type = SDL_MOUSEWHEEL;
	sdl_event.wheel.x = e->angleDelta().x() / 8;
	sdl_event.wheel.y = e->angleDelta().y() / 8;
	sdl_event.wheel.timestamp = e->timestamp();
	sdl_event.wheel.direction = SDL_MOUSEWHEEL_NORMAL;
	SDL_PushEvent(&sdl_event);	
}

void InputTranslator::qt_mouse_move(QMouseEvent * e)
{
	SDL_Event sdl_event;
	sdl_event.type = SDL_MOUSEMOTION;
	sdl_event.motion.timestamp = e->timestamp();	
	sdl_event.motion.x = e->pos().x();
	sdl_event.motion.y = e->pos().y();
	sdl_event.motion.state = _convert_Qtmb_modifier_to_SDL(e->buttons());
	SDL_PushEvent(&sdl_event);	
}

void InputTranslator::qt_mouse_press(QMouseEvent * e)
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

void InputTranslator::qt_mouse_release(QMouseEvent * e)
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

void InputTranslator::qt_window_resize(QResizeEvent * e)
{
    if(GetSubsystem<Engine>()->IsInitialized())
	{
		int width = e->size().width();
		int height = e->size().height();

		Graphics* graphics = GetSubsystem<Graphics>();

		SDL_Window * win = (SDL_Window*)graphics->GetWindow();
		SDL_SetWindowSize(win, width, height);
	}	
}


void InputTranslator::handle_key_down(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	trigger_condition tc;
	tc.key = int16_t(eventData["Key"].GetInt());
	tc.mouse_button = 0;
	int8_t qualifiers(eventData["Qualifiers"].GetInt());
	int8_t mouse_buttons(eventData["Buttons"].GetInt());
	
	for (int i = m_context_stack.Size(); i > 0; --i)
	{
		input_context * cur_ic = m_context_stack[i-1];
		input_action_trigger * trig = nullptr;

		// Go through every trigger - if the current mb and quals are contained in the required qual and mb fields
		// and the allowed 
		// First find all triggers with the exact match for key and mouse qualifiers
		trigger_range tr = cur_ic->get_triggers(tc);
		
		while (tr.first != tr.second)
		{
			input_action_trigger * trig = tr.first->second;

			// Check the qualifier and mouse button required conditions
			bool pass_qual_required((trig->qual_required & qualifiers) == trig->qual_required);
			bool pass_mb_required((trig->mb_required & mouse_buttons) == trig->mb_required);

			// Check the qualifier and mouse button allowed conditions
			int8_t allowed_quals = trig->qual_required | trig->qual_allowed;
			int8_t allowed_mb = trig->mb_required | trig->mb_allowed;
			bool pass_qual_allowed(((allowed_quals & QUAL_ANY) == QUAL_ANY) || (qualifiers | allowed_quals) == allowed_quals);
			bool pass_mb_allowed(((allowed_mb & MOUSEB_ANY) == MOUSEB_ANY) || ((mouse_buttons | allowed_mb) == allowed_mb));

			// If passes all the conditions, send the event for the trigger and mark trigger as active
			if (!_trigger_already_active(trig) && pass_qual_required && pass_mb_required && pass_qual_allowed && pass_mb_allowed)
			{
				if ((trig->trigger_state & t_begin) == t_begin)
				{
					using namespace InputTrigger;
					VariantMap event_data;
					event_data[P_TRIGGER_NAME] = trig->name;
					event_data[P_TRIGGER_STATE] = t_begin;
					event_data[P_NORM_MPOS] = current_norm_mpos;
					event_data[P_NORM_MDELTA] = Vector2();
					event_data[P_MOUSE_WHEEL] = 0;
					SendEvent(E_INPUT_TRIGGER,event_data);
				}
				active_triggers.Push(trig);
			}

			++tr.first;
		}
	}	
}

bool InputTranslator::_trigger_already_active(input_action_trigger * trig)
{
	auto iter = active_triggers.Begin();
	while (iter != active_triggers.End())
	{
		if (*trig == **iter)
			return true;
		++iter;
	}
	return false;		
}

void InputTranslator::handle_key_up(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	trigger_condition tc;
	tc.key = int16_t(eventData["Key"].GetInt());
	tc.mouse_button = 0;
	int8_t mouse_buttons = int8_t(eventData["Buttons"].GetInt());
	int8_t qualifiers = int8_t(eventData["Qualifiers"].GetInt());

	auto iter = active_triggers.Begin();
	while (iter != active_triggers.End())
	{
		input_action_trigger * curt = *iter;

		// Always end the action and send the trigger when the main key is depressed - dont care
		// about any of the qualifier situation
		if (tc.key == curt->condition.key)
		{
			if ((curt->trigger_state & t_end) == t_end)
			{
				VariantMap event_data;
				using namespace InputTrigger;
				event_data[P_TRIGGER_NAME] = curt->name;
				event_data[P_TRIGGER_STATE] = t_end;
				event_data[P_NORM_MPOS] = current_norm_mpos;
				event_data[P_NORM_MDELTA] = Vector2();
				event_data[P_MOUSE_WHEEL] = 0;
				SendEvent(E_INPUT_TRIGGER,event_data);
			}
			iter = active_triggers.Erase(iter);
		}
		else
		{
			++iter;
		}
	}

}


void InputTranslator::_normalize_mpos(Vector2 & to_norm)
{
	int width_pixels = bbtk.scene_view()->width();
	int height_pixels = bbtk.scene_view()->height();

	to_norm.x_ /= float(width_pixels);
	to_norm.y_ /= float(height_pixels);
}

void InputTranslator::handle_mouse_down(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	trigger_condition tc;
	tc.key = 0;
	tc.mouse_button = eventData["Button"].GetInt();
	int8_t mouse_buttons = int8_t(eventData["Buttons"].GetInt());
	int8_t qualifiers = int8_t(eventData["Qualifiers"].GetInt());
	int8_t wheel = 0;
	Vector2 norm_mdelta;

	if (tc.mouse_button == MOUSEB_WHEEL)
		wheel = int8_t(eventData["Wheel"].GetInt());

	if (tc.mouse_button == MOUSEB_MOVE)
	{
		Vector2 current_mpos(float(eventData["X"].GetInt()),float(eventData["Y"].GetInt()));
		_normalize_mpos(current_mpos);
		norm_mdelta = current_mpos - current_norm_mpos;
		current_norm_mpos = current_mpos;
	}


	for (int i = m_context_stack.Size(); i > 0; --i)
	{
		input_context * cur_ic = m_context_stack[i-1];
		input_action_trigger * trig = nullptr;

		// Go through every trigger - if the current mb and quals are contained in the required qual and mb fields
		// and the allowed 
		// First find all triggers with the exact match for key and mouse qualifiers
		trigger_range tr = cur_ic->get_triggers(tc);
		while (tr.first != tr.second)
		{
			input_action_trigger * trig = tr.first->second;

			// Check the qualifier and mouse button required conditions
			bool pass_qual_required((trig->qual_required & qualifiers) == trig->qual_required);
			bool pass_mb_required((trig->mb_required & mouse_buttons) == trig->mb_required);

			// Check the qualifier and mouse button allowed conditions
			int8_t allowed_quals = trig->qual_required | trig->qual_allowed;
			int8_t allowed_mb = trig->mb_required | trig->mb_allowed | trig->condition.mouse_button;
			bool pass_qual_allowed(((allowed_quals & QUAL_ANY) == QUAL_ANY) || (qualifiers | allowed_quals) == allowed_quals);
			bool pass_mb_allowed(((allowed_mb & MOUSEB_ANY) == MOUSEB_ANY) || ((mouse_buttons | allowed_mb) == allowed_mb));

			// If passes all the conditions, send the event for the trigger and mark trigger as active

			if (!_trigger_already_active(trig) && pass_qual_required && pass_mb_required && pass_qual_allowed && pass_mb_allowed)
			{
				if ((trig->trigger_state & t_begin) == t_begin)
				{
					VariantMap event_data;
					using namespace InputTrigger;
					event_data[P_TRIGGER_NAME] = trig->name;
					event_data[P_TRIGGER_STATE] = t_begin;
					event_data[P_NORM_MPOS] = current_norm_mpos;
					event_data[P_NORM_MDELTA] = norm_mdelta;
					event_data[P_MOUSE_WHEEL] = wheel;
					SendEvent(E_INPUT_TRIGGER,event_data);
				}

				if (tc.mouse_button != MOUSEB_WHEEL && tc.mouse_button != MOUSEB_MOVE)
					active_triggers.Push(trig);
			}

			++tr.first;
		}
	}	

}

void InputTranslator::handle_mouse_up(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	trigger_condition tc;
	tc.key = 0;
	tc.mouse_button = eventData["Button"].GetInt();
	int8_t mouse_buttons = int8_t(eventData["Buttons"].GetInt());
	int8_t qualifiers = int8_t(eventData["Qualifiers"].GetInt());

	auto iter = active_triggers.Begin();
	while (iter != active_triggers.End())
	{
		input_action_trigger * curt = *iter;

		if (tc.mouse_button == curt->condition.mouse_button)
		{
			if (((curt->trigger_state & t_end) == t_end))
			{
				VariantMap event_data;
				using namespace InputTrigger;
				event_data[P_TRIGGER_NAME] = curt->name;
				event_data[P_TRIGGER_STATE] = t_end;
				event_data[P_NORM_MPOS] = current_norm_mpos;
				event_data[P_NORM_MDELTA] = Vector2();
				event_data[P_MOUSE_WHEEL] = 0;
				SendEvent(E_INPUT_TRIGGER,event_data);
			}
			iter = active_triggers.Erase(iter);
		}
		else
		{
			++iter;
		}
	}

}

void InputTranslator::handle_mouse_wheel(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	eventData["Button"] = MOUSEB_WHEEL;
	handle_mouse_down(eventType, eventData);
}

void InputTranslator::handle_mouse_move(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	eventData["Button"] = MOUSEB_MOVE;
	handle_mouse_down(eventType, eventData);
}


void InputTranslator::push_context(input_context * ctxt)
{
	m_context_stack.Push(ctxt);
}

void InputTranslator::pop_context()
{
	m_context_stack.Pop();		
}

void InputTranslator::_init_key_map()
{
    m_keymap[Qt::Key_unknown]     = SDLK_UNKNOWN;
    m_keymap[Qt::Key_Escape]      = SDLK_ESCAPE;
    m_keymap[Qt::Key_Tab]         = SDLK_TAB;
    m_keymap[Qt::Key_Backspace]   = SDLK_BACKSPACE;
    m_keymap[Qt::Key_Return]      = SDLK_RETURN;
    m_keymap[Qt::Key_Enter]       = SDLK_KP_ENTER;
    m_keymap[Qt::Key_Insert]      = SDLK_INSERT;
    m_keymap[Qt::Key_Delete]      = SDLK_DELETE;
    m_keymap[Qt::Key_Pause]       = SDLK_PAUSE;
    m_keymap[Qt::Key_Print]       = SDLK_PRINTSCREEN;
    m_keymap[Qt::Key_SysReq]      = SDLK_SYSREQ;
    m_keymap[Qt::Key_Home]        = SDLK_HOME;
    m_keymap[Qt::Key_End]         = SDLK_END;
    m_keymap[Qt::Key_Left]        = SDLK_LEFT;
    m_keymap[Qt::Key_Right]       = SDLK_RIGHT;
    m_keymap[Qt::Key_Up]          = SDLK_UP;
    m_keymap[Qt::Key_Down]        = SDLK_DOWN;
    m_keymap[Qt::Key_PageUp]      = SDLK_PAGEUP;
    m_keymap[Qt::Key_PageDown]    = SDLK_PAGEDOWN;
    m_keymap[Qt::Key_Shift]       = SDLK_LSHIFT;
    m_keymap[Qt::Key_Control]     = SDLK_LCTRL;
    m_keymap[Qt::Key_Alt]         = SDLK_LALT;
    m_keymap[Qt::Key_CapsLock]    = SDLK_CAPSLOCK;
    m_keymap[Qt::Key_NumLock]     = SDLK_NUMLOCKCLEAR;
    m_keymap[Qt::Key_ScrollLock]  = SDLK_SCROLLLOCK;
    m_keymap[Qt::Key_F1]          = SDLK_F1;
    m_keymap[Qt::Key_F2]          = SDLK_F2;
    m_keymap[Qt::Key_F3]          = SDLK_F3;
    m_keymap[Qt::Key_F4]          = SDLK_F4;
    m_keymap[Qt::Key_F5]          = SDLK_F5;
    m_keymap[Qt::Key_F6]          = SDLK_F6;
    m_keymap[Qt::Key_F7]          = SDLK_F7;
    m_keymap[Qt::Key_F8]          = SDLK_F8;
    m_keymap[Qt::Key_F9]          = SDLK_F9;
    m_keymap[Qt::Key_F10]         = SDLK_F10;
    m_keymap[Qt::Key_F11]         = SDLK_F11;
    m_keymap[Qt::Key_F12]         = SDLK_F12;
    m_keymap[Qt::Key_F13]         = SDLK_F13;
    m_keymap[Qt::Key_F14]         = SDLK_F14;
    m_keymap[Qt::Key_F15]         = SDLK_F15;
    m_keymap[Qt::Key_Menu]        = SDLK_MENU;
    m_keymap[Qt::Key_Help]        = SDLK_HELP;

	for(int key='A'; key<='Z'; key++)
		m_keymap[Qt::Key(key)] = key + 32;

	for(int key='0'; key<='9'; key++)
		m_keymap[Qt::Key(key)] = key;
}

void InputTranslator::_init_mouse_button_map()
{
    m_mouse_button_map[Qt::LeftButton]     = SDL_BUTTON_LEFT;
    m_mouse_button_map[Qt::RightButton]    = SDL_BUTTON_RIGHT;
    m_mouse_button_map[Qt::MiddleButton]   = SDL_BUTTON_MIDDLE;
    m_mouse_button_map[Qt::XButton1]       = SDL_BUTTON_X1;
    m_mouse_button_map[Qt::XButton2]       = SDL_BUTTON_X2;
}

int InputTranslator::_convert_Qtkey_to_SDL(int qtKey)
{
	auto iter = m_keymap.Find(qtKey);
	if (iter != m_keymap.End())
		return iter->second_;
	return 0;
}

int InputTranslator::_convert_Qtkey_modifier_to_SDL(Qt::KeyboardModifiers qtKeyModifiers)
{
    int sdlModifiers = KMOD_NONE;
	int64_t flg = qtKeyModifiers;
    if((flg & Qt::ShiftModifier) == Qt::ShiftModifier)
	{
		sdlModifiers |= KMOD_SHIFT;
	}
	if((flg & Qt::ControlModifier) == Qt::ControlModifier)
	{
		sdlModifiers |= KMOD_CTRL;
	}
    if((flg & Qt::AltModifier) == Qt::AltModifier)
	{
		sdlModifiers |= KMOD_ALT;
	}

	return sdlModifiers;
}

int InputTranslator::_convert_Qtmb_modifier_to_SDL(Qt::MouseButtons buttons)
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

int InputTranslator::_convert_Qtmb_to_SDL(int qtbutton)
{
	auto iter = m_mouse_button_map.Find(qtbutton);
	if (iter != m_mouse_button_map.End())
		return iter->second_;
	return 0;
}

