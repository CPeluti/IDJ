#pragma once

#include <sstream>
#include <string>
#include <functional>

#define BIND_EVENT_FN(fn) [this](auto &&...args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#define EVENT_TYPE(type)                                                        \
    static EventType GetStaticType() { return EventType::type; }                \
    virtual EventType GetEventType() const override { return GetStaticType(); } \
    virtual const char *GetName() const override { return #type; }

enum class EventType
{
    None = 0,
    OnCollision,
    OnDamageTaken,
    OnDeath,
    OnInteraction,
    OnEffect,
    OnCastSpell,
};

class Event
{
    friend class EventDispatcher;

public:
    virtual EventType GetEventType() const = 0;
    virtual const char *GetName() const = 0;
    virtual std::string ToString() const { return GetName(); }

protected:
    bool m_Handled = false;
};

class EventDispatcher
{
    template <typename T>
    using EventFn = std::function<bool(T &)>;

public:
    EventDispatcher(Event &e) : m_Event(e) {}

    template <typename T>
    bool Dispatch(EventFn<T> func)
    {
        if (m_Event.GetEventType() == T::GetStaticType())
        {
            m_Event.m_Handled = func(*(T *)&m_Event);
            return true;
        }
        return false;
    }

private:
    Event &m_Event;
};

inline std::string format_as(const Event &e)
{
    return e.ToString();
}