#pragma once

#include "Event.h"
#include <memory>
#include <vector>
#include "Game/Effect.h"

class GameObject;
class OnCollisionEvent : public Event
{
public:
    OnCollisionEvent(GameObject &gameObject) : m_GameObject(gameObject) {}

    EVENT_TYPE(OnCollision);

    inline GameObject &GetGameObject() const { return m_GameObject; }

protected:
    GameObject &m_GameObject;
};

class OnDamageTakenEvent : public Event
{
public:
    OnDamageTakenEvent(GameObject &dealer, float amount) : m_Dealer(dealer), m_Amount(amount) {}

    EVENT_TYPE(OnDamageTaken);

    inline GameObject &GetDealer() const { return m_Dealer; }
    inline float GetAmount() const { return m_Amount; }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "teste de dano: " << m_Amount;
        return ss.str();
    }

protected:
    GameObject &m_Dealer;
    float m_Amount;
};

class OnCastSpellEvent : public Event
{
public:
    OnCastSpellEvent(float amount) : m_Amount(amount) {}

    EVENT_TYPE(OnCastSpell);

    inline float GetAmount() const { return m_Amount; }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "teste de cast cost: " << m_Amount;
        return ss.str();
    }

protected:
    float m_Amount;
};


class OnDeathEvent : public Event
{
public:
    OnDeathEvent() {}

    EVENT_TYPE(OnDeath);
};

enum class InteractionType
{
    PickUp,
    Effect,
    Talk,
    None = 0
};

class OnInteractionEvent : public Event
{
public:
    OnInteractionEvent(GameObject &gameObject, InteractionType type) : m_GameObject(gameObject), m_InteractionType(type) {}

    EVENT_TYPE(OnInteraction);

    inline GameObject &GetGameObject() const { return m_GameObject; }
    inline InteractionType GetInteractionType() const { return m_InteractionType; }

protected:
    GameObject &m_GameObject;
    InteractionType m_InteractionType;
};


template <typename EffectType>
class OnEffectEvent : public Event
{
public:
    OnEffectEvent(std::vector<Effect<EffectType>*> effects) : m_Effects(effects) {}

    EVENT_TYPE(OnEffect);

    inline std::vector<Effect<EffectType>*> GetEffects() const { return m_Effects; }

protected:
    std::vector<Effect<EffectType>*> m_Effects;
};
