#define INCLUDE_SDL
#include "Core/SDL_include.h"
#include "Game/Lifebar.h"
#include "Game/HealthSystem.h"

HealthSystem::HealthSystem(GameObject &associated, float hp) : Component(associated), hp(hp)
{
    m_maxHp = hp;
    associated.subject.addObserver(this);
    auto lb = std::make_shared<Lifebar>(associated, (int)m_maxHp, Vec2(associated.box.GetSize().x, 4.0), Vec2(0, (int)associated.box.GetSize().y / 4));
    associated.AddComponent(lb);
    m_lifebar = lb;
}

float HealthSystem::TakeDamage(float damage)
{
    hp -= damage;
    if(auto lifebar = m_lifebar.lock()){
        lifebar->setAmount(hp);
    }
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
