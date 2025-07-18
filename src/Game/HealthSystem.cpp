#define INCLUDE_SDL
#include "Core/SDL_include.h"
#include "Core/SpriteRenderer.h"
#include "Core/Game.h"
#include "Core/Camera.h"
#include "Game/Lifebar.h"
#include "Game/HealthSystem.h"

HealthSystem::HealthSystem(GameObject &associated, float hp, bool boss) : Component(associated), hp(hp), m_boss(boss)
{
    m_maxHp = hp;
    if (boss) {
        std::shared_ptr<GameObject> lifebar = std::make_shared<GameObject>();
		spriteLifebar = Game::GetInstance().GetCurrentState()->AddObject(lifebar);
        associated.subject.addObserver(this);
        std::shared_ptr<SpriteRenderer> sr = std::make_shared<SpriteRenderer>(*lifebar, "resources/interface/barra_boss.png");
        sr->SetCameraFollower(true);
        lifebar->AddComponent(sr);
        sr->z = 1000; // Ensure the boss lifebar is rendered above other elements
        lifebar->box.Move(Vec2{ (Game::GetInstance().GetWindowSize().x / 2)/Camera::zoom , 15.0 });
        auto lb = std::make_shared<Lifebar>(*lifebar, (int)m_maxHp, Vec2{231,12 }, Vec2{8,14}, SDL_Color{255,0,0,255}, boss);
        lifebar->AddComponent(lb);
        m_lifebar = lb;
    }
    else {
        associated.subject.addObserver(this);
        auto lb = std::make_shared<Lifebar>(associated, (int)m_maxHp, Vec2(associated.box.GetSize().x, 4.0), Vec2(0, (int)associated.box.GetSize().y / 3));
        associated.AddComponent(lb);
        m_lifebar = lb;
    }
}
HealthSystem::~HealthSystem()
{
    if (auto lifebar = spriteLifebar.lock()) {
        lifebar->RequestDelete();
    }
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
