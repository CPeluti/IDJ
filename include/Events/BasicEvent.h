#pragma once

#include "Event.h"

class GameObject;
class OnCollisionEvent : public Event {
    public:
        OnCollisionEvent(GameObject& gameObject): m_GameObject(gameObject){}
        static EventType GetStaticType() { return EventType::OnCollision; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char* GetName() const override { return "OnCollision"; }

        inline GameObject& GetGameObject() const { return m_GameObject; }
    protected:
        GameObject& m_GameObject;
};