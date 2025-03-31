#pragma once

#include "Event.h"

class GameObject;
class OnCollisionEvent : public Event {
    public:
        OnCollisionEvent(GameObject& gameObject): m_GameObject(gameObject){}

        EVENT_TYPE(OnCollision);

        inline GameObject& GetGameObject() const { return m_GameObject; }
    protected:
        GameObject& m_GameObject;
};

class OnDamageTakenEvent : public Event {
    public:
        OnDamageTakenEvent(GameObject& dealer, float amount): m_Dealer(dealer), m_Amount(amount){}

        EVENT_TYPE(OnDamageTakenEvent);

        inline GameObject& GetDealer() const { return m_Dealer; }
        inline float GetAmount() const { return m_Amount; }

    protected:
        GameObject& m_Dealer;
        float m_Amount;
};

class OnDeathEvent : public Event {
    public:
        OnDeathEvent() {}

        EVENT_TYPE(OnDeathEvent);

};
