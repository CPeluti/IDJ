#define INCLUDE_SDL
#include "Core/SDL_include.h"

#include "Game/HealthSystem.h"

HealthSystem::HealthSystem(GameObject& associated, float hp):Component(associated), hp(hp){
    associated.subject.addObserver(this);
}

float HealthSystem::TakeDamage(float damage){
    hp -= damage;
    return damage;
}

void HealthSystem::OnEvent(Event& evt){
    EventDispatcher dispatcher(evt);

    dispatcher.Dispatch<OnDamageTakenEvent>(BIND_EVENT_FN(HealthSystem::OnDamageTaken));
} 

bool HealthSystem::OnDamageTaken(OnDamageTakenEvent& evt){
    if(&evt.GetDealer() != &this->associated){
        TakeDamage(evt.GetAmount());
    }
    return true;
}
