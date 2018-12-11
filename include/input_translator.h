#pragma once

#include <Urho3D/Core/Object.h>
#include <Urho3D/Input/InputEvents.h>
#include <math_utils.h>
#include <map>
#include <vector>

static const int32_t MOUSEB_MOVE = 32;
static const int32_t MOUSEB_WHEEL = 64;

#ifdef QT_BUILD
#include <QObject>
class QKeyEvent;
class QWheelEvent;
class QResizeEvent;
class QMouseEvent;
#endif

enum Trigger_State
{
    T_BEGIN = 1,
    T_END = 2
};

union Trigger_Condition
{
    struct
    {
        int32_t key_; // 0 is no key - if no key then mb_mask must be non-zero
        int32_t mouse_button_;
    };
    int64_t lookup_key_;

    bool operator==(const Trigger_Condition & rhs)
    {
        return (lookup_key_ == rhs.lookup_key_);
    }
};

struct Input_Action_Trigger
{
    Urho3D::StringHash name_;
    int32_t trigger_state_;
    Trigger_Condition condition_; // Here for convenience
    int32_t mb_required_;         // 0 is no button
    int32_t qual_required_;       // 0 is no keyboard qualifier
    int32_t
        mb_allowed_; // If MOUSEB_ANY is included int the mouse_buttons, these mouse buttons will cause the state to end or not begin
    int32_t qual_allowed_; // Same thing as above with QUAL_ANY for key qualifiers

    bool operator==(const Input_Action_Trigger & rhs)
    {
        return (condition_ == rhs.condition_ && mb_required_ == rhs.mb_required_ &&
                qual_required_ == rhs.qual_required_ && trigger_state_ == rhs.trigger_state_);
    }
};

using Trigger_Map = std::multimap<int64_t, Input_Action_Trigger *>;
using Trigger_Range = std::pair<Trigger_Map::iterator, Trigger_Map::iterator>;

struct Input_Context
{
    bool add_trigger(Input_Action_Trigger * trigger);

    Input_Action_Trigger * create_trigger(const Input_Action_Trigger & trig);

    bool destroy_trigger(Input_Action_Trigger * trigger);

    bool destroy_triggers(const Trigger_Condition & cond);

    Input_Action_Trigger * remove_trigger(Input_Action_Trigger * trigger);

    std::vector<Input_Action_Trigger *> remove_triggers(const Trigger_Condition & cond);

    Trigger_Range get_triggers(const Trigger_Condition & cond);

    Trigger_Map t_map_;
};

using Context_Map = Urho3D::HashMap<Urho3D::StringHash, Input_Context *>;

class Input_Map
{
  public:
    Input_Map();

    Input_Map(const Input_Map & copy_);

    ~Input_Map();

    Input_Map & operator=(Input_Map rhs);

    bool add_context(const Urho3D::StringHash & name, Input_Context * to_add);

    Input_Context * get_context(const Urho3D::StringHash & name);

    Input_Context * create_context(const Urho3D::StringHash & name);

    bool destroy_context(const Urho3D::StringHash & name);

    bool destroy_context(Input_Context * to_destroy);

    Input_Context * remove_context(const Urho3D::StringHash & name);

    Input_Context * remove_context(Input_Context * to_remove);

    bool rename_context(const Urho3D::StringHash & old_name, const Urho3D::StringHash & new_name);

  private:
    Context_Map contexts_;
};

struct Viewport_Info
{
    int vp_index;
    fvec2 vp_norm_mpos;
    fvec2 vp_norm_mdelta;
};

// Keep track of current modifier state etc?
class Input_Translator : public Urho3D::Object
{
    URHO3D_OBJECT(Input_Translator, Urho3D::Object)

  public:
    Input_Translator(Urho3D::Context * context);
    ~Input_Translator();

#ifdef QT_BUILD
    void qt_key_press(QKeyEvent * e);
    void qt_key_release(QKeyEvent * e);
    void qt_mouse_wheel(QWheelEvent * e);
    void qt_mouse_move(QMouseEvent * e);
    void qt_mouse_press(QMouseEvent * e);
    void qt_mouse_release(QMouseEvent * e);
    void qt_window_resize(QResizeEvent * e);
#endif

    void init();

    void release();

    void handle_key_down(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);
    void handle_key_up(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);
    void handle_mouse_down(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);
    void handle_mouse_up(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);
    void handle_mouse_wheel(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);
    void handle_mouse_move(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data);

    void push_context(Input_Context * ctxt);
    void pop_context();

  private:
    bool _trigger_already_active(Input_Action_Trigger * trig);

    void _normalize_mpos(fvec2 & to_norm);

    void _fill_vp_info(Urho3D::Vector<Viewport_Info> & vp_inf_vec, const fvec2 & norm_mpos, const fvec2 & norm_mdelta);    

#ifdef QT_BUILD
	void _init_key_map();
	void _init_mouse_button_map();
    int _convert_Qtkey_modifier_to_SDL(Qt::KeyboardModifiers qtKeyModifiers);
    int _convert_Qtmb_modifier_to_SDL(Qt::MouseButtons buttons);
    int _convert_Qtmb_to_SDL(int qtbutton);
    int _convert_Qtkey_to_SDL(int qtKey);
    Urho3D::HashMap<int, int> keymap_;
    Urho3D::HashMap<int, int> mouse_button_map_;
#endif

    Urho3D::Vector<Input_Context *> context_stack_;
    Urho3D::Vector<Input_Action_Trigger *> active_triggers_;
    fvec2 current_norm_mpos_;
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
    URHO3D_PARAM(P_VIEWPORT_NORM_MPOS, viewport_normalized_mouse_pos); // Vector2
    URHO3D_PARAM(P_VIEWPORT_NORM_MDELTA, viewport_normalized_mouse_delta); // Vector2
    URHO3D_PARAM(P_VIEWPORT_INDEX, viewport_index); // int
    URHO3D_PARAM(P_MOUSE_WHEEL, mouse_wheel);            // int
}

} // namespace Urho3D