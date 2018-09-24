#ifndef INPUT_TRANSLATOR_H
#define INPUT_TRANSLATOR_H

#include <Urho3D/Core/Object.h>
#include <Urho3D/Input/InputEvents.h>
#include <QObject>

static const int32_t MOUSEB_MOVE = 32;
static const int32_t MOUSEB_WHEEL = 64;
static const int32_t MOUSEB_ANY = 128;

enum trigger_state
{
	t_begin=1,
	t_end=2
};

union trigger_condition
{
	struct
	{
		int32_t key; // 0 is no key - if no key then mb_mask must be non-zero
		int32_t mouse_button;
	};
	int64_t lookup_key;

	bool operator==(const trigger_condition & rhs)
	{
		return (lookup_key == rhs.lookup_key);
	}
};
	
struct input_action_trigger
{
	Urho3D::StringHash name;
	int32_t trigger_state;
	trigger_condition condition; // Here for convenience
	int32_t mb_required; // 0 is no button
	int32_t qual_required; // 0 is no keyboard qualifier
	int32_t mb_allowed; // If MOUSEB_ANY is included int the mouse_buttons, these mouse buttons will cause the state to end or not begin
	int32_t qual_allowed; // Same thing as above with QUAL_ANY for key qualifiers

	bool operator==(const input_action_trigger & rhs)
	{
		return (condition == rhs.condition && mb_required == rhs.mb_required && qual_required == rhs.qual_required);
	}
};

typedef std::multimap<int64_t, input_action_trigger*> trigger_map;
typedef std::pair<trigger_map::iterator,trigger_map::iterator> trigger_range;

struct input_context
{
	bool add_trigger(
		input_action_trigger * trigger
		);

	input_action_trigger * create_trigger(
		const trigger_condition & cond
		);

	bool destroy_trigger(
		input_action_trigger * trigger
		);

	bool destroy_triggers(
		const trigger_condition & cond
		);

	input_action_trigger * remove_trigger(
		input_action_trigger * trigger
		);

	std::vector<input_action_trigger*> remove_triggers(
		const trigger_condition & cond
		);
	
	trigger_range get_triggers(
		const trigger_condition & cond
		);
	
	trigger_map t_map;
};

typedef Urho3D::HashMap<Urho3D::StringHash, input_context*> context_map;

class InputMap
{
  public:

	InputMap();

	InputMap(const InputMap & copy_);

    ~InputMap();

	InputMap & operator=(InputMap rhs);

	bool add_context(const Urho3D::StringHash & name, input_context * to_add);

	input_context * get_context(const Urho3D::StringHash & name);
	
	input_context * create_context(const Urho3D::StringHash & name);

	bool destroy_context(const Urho3D::StringHash & name);

	bool destroy_context(input_context * to_destroy);

	input_context * remove_context(const Urho3D::StringHash & name);

	input_context * remove_context(input_context * to_remove);

	bool rename_context(const Urho3D::StringHash & old_name, const Urho3D::StringHash & new_name);

  private:	
	context_map m_contexts;
};

class QKeyEvent;
class QWheelEvent;
class QResizeEvent;
class QMouseEvent;

// Keep track of current modifier state etc?
class InputTranslator : public Urho3D::Object
{
	URHO3D_OBJECT(InputTranslator, Urho3D::Object)
	
  public:

	InputTranslator(Urho3D::Context * context);
	~InputTranslator();

	void qt_key_press(QKeyEvent * e);
	void qt_key_release(QKeyEvent * e);
	void qt_mouse_wheel(QWheelEvent * e);
	void qt_mouse_move(QMouseEvent * e);
	void qt_mouse_press(QMouseEvent * e);
	void qt_mouse_release(QMouseEvent * e);
	void qt_window_resize(QResizeEvent * e);

	void handle_key_down(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void handle_key_up(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void handle_mouse_down(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void handle_mouse_up(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void handle_mouse_wheel(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void handle_mouse_move(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	void push_context(input_context * ctxt);
	void pop_context();

  private:

	void _init_key_map();
	void _init_mouse_button_map();

	bool _trigger_already_active(input_action_trigger * trig);

	void _normalize_mpos(Urho3D::Vector2 & to_norm);
	
	int _convert_Qtkey_modifier_to_SDL(Qt::KeyboardModifiers qtKeyModifiers);
	int _convert_Qtmb_modifier_to_SDL(Qt::MouseButtons buttons);
	int _convert_Qtmb_to_SDL(int qtbutton);
	int _convert_Qtkey_to_SDL(int qtKey);

	Urho3D::Vector<input_context*> m_context_stack;
	Urho3D::HashMap<int, int> m_keymap;
	Urho3D::HashMap<int, int> m_mouse_button_map;
	Urho3D::Vector<input_action_trigger*> active_triggers;
	Urho3D::Vector2 current_norm_mpos;
};

namespace Urho3D
{

/// Input Trigger Event
URHO3D_EVENT(E_INPUT_TRIGGER, InputTrigger)
{
    URHO3D_PARAM(P_TRIGGER_NAME, trigger_name);          // StringHash	
    URHO3D_PARAM(P_TRIGGER_STATE, trigger_state);        // int
    URHO3D_PARAM(P_NORM_MPOS, normalized_mouse_pos);     // Vector2
    URHO3D_PARAM(P_NORM_MDELTA, normalized_mouse_delta); // Vector2
	URHO3D_PARAM(P_MOUSE_WHEEL, mouse_wheel);            // int
	
}

}

#endif
