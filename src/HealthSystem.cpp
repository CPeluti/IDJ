#include "Core/HealthSystem.h"
#define INCLUDE_SDL
#include "Core/SDL_include.h"
#include "Core/Lifebar.h"

HealthSystem::HealthSystem(GameObject &associated, float hp) : Component(associated), hp(hp)
{
    m_maxHp = hp;
    associated.subject.addObserver(this);

    m_lifebar = new Lifebar(associated, (int)m_maxHp, {associated.box.GetSize().x, (float)10}, {0, (int)associated.box.GetSize().y / 4});
    associated.AddComponent(m_lifebar);
}

float HealthSystem::TakeDamage(float damage)
{
    hp -= damage;
    m_lifebar->setAmount(damage);
    return damage;
}

void HealthSystem::OnEvent(Event &evt)
{
    EventDispatcher dispatcher(evt);

    dispatcher.Dispatch<OnDamageTakenEvent>(BIND_EVENT_FN(HealthSystem::OnDamageTaken));
}

bool HealthSystem::OnDamageTaken(OnDamageTakenEvent &evt)
{
    if (&evt.GetDealer() != &this->associated)
    {
        TakeDamage(evt.GetAmount());
    }
    return true;
}
