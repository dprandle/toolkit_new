#pragma once

#include <Urho3D/Urho3DAll.h>
#include <mtdebug_print.h>

using namespace Urho3D;

struct Func_Holder
{
    virtual void handle_event(StringHash event_type, VariantMap & event_data) = 0;

    virtual ~Func_Holder() {}
};

template<class T>
struct Func_Holder_Impl : public Func_Holder
{
    void handle_event(StringHash event_type, VariantMap & event_data)
    {
        (obj->*func)(event_type,event_data);
    }

    T * obj;
    void (T::*func)(StringHash, VariantMap&);
};


class Urho_Event_Receiver : public Object
{
    URHO3D_OBJECT(Urho_Event_Receiver, Object)

    public:

    Urho_Event_Receiver(Context * ctxt, StringHash event_id):
        Object(ctxt)
    {
        SubscribeToEvent(event_id, URHO3D_HANDLER(Urho_Event_Receiver, handle_event));
        func_map[event_id] = Vector<Func_Holder*>();
    }

    Urho_Event_Receiver(Context * ctxt, HashSet<StringHash> event_ids):
        Object(ctxt)
    {
        auto iter = event_ids.Begin();
        while (iter != event_ids.End())
        {
            SubscribeToEvent(*iter, URHO3D_HANDLER(Urho_Event_Receiver, handle_event));
            func_map[*iter] = Vector<Func_Holder*>();
            ++iter;
        }
    }

    ~Urho_Event_Receiver()
    {
        UnsubscribeFromAllEvents();
        while (func_map.Begin() != func_map.End())
        {
            while (func_map.Begin()->second_.Size() > 0)
            {
                delete func_map.Begin()->second_.Back();
                func_map.Begin()->second_.Pop();
            }
            func_map.Erase(func_map.Begin());
        }
    }

    template<class T>
    bool push_function(StringHash event_type, T * obj, void (T::*func)(StringHash, VariantMap&))
    {
        auto iter = func_map.Find(event_type);
        bool ret = false;
        if (iter != func_map.End())
        {
            Func_Holder_Impl<T> * f = new Func_Holder_Impl<T>;
            f->obj = obj;
            f->func = func;
            iter->second_.Push(f);
            ret = true;
        }
        return ret;
    }

    bool pop_function(StringHash event_type)
    {
        auto iter = func_map.Find(event_type);
        if (iter != func_map.End())
        {
            iter->second_.Pop();
            return true;
        }
        return false;
    }

    int function_count(StringHash event_type)
    {
        auto iter = func_map.Find(event_type);
        if (iter != func_map.End())
            return iter->second_.Size();
        return -1;
    }

    void handle_event(StringHash event_type, VariantMap & event_data)
    {
        auto iter = func_map.Find(event_type);
        if (iter != func_map.End())
        {
            Vector<Func_Holder*> & funcs = iter->second_;
            for (int i = 0; i < funcs.Size(); ++i)
                funcs[i]->handle_event(event_type, event_data);
        }
    }

    private:

    HashMap<StringHash, Vector<Func_Holder *>> func_map;
    
};

using fvec2 = Vector2;
using fvec3 = Vector3;
using fvec4 = Vector4;
using ivec2 = IntVector2;
using ivec3 = IntVector3;
using irect = IntRect;
using frect = Rect;

using fquat = Quaternion;
using fmat2 = Matrix2;
using fmat3 = Matrix3;
using fmat4 = Matrix4;
using fmat3x4 = Matrix3x4;

using ustr = String;

using uint8 = uint8_t;
using int8 = int8_t;
using uint16 = uint16_t;
using int16 = int16_t;
using uint32 = uint32_t;
using int32 = int32_t;
using uint64 = uint64_t;
using int64 = int64_t;