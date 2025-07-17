#define INCLUDE_SDL
#include "Core/SDL_include.h"
#include "Game/Lifebar.h"
#include "Game/ManaSystem.h"

ManaSystem::ManaSystem(GameObject& associated, float mana) : Component(associated), m_mana(mana), m_manaRegenTimer(5.0f), m_manaPerIncrease(10.0f)
{
    m_maxMana = mana;
    associated.subject.addObserver(this);
    auto manabar = std::make_shared<Lifebar>(associated, (int)m_maxMana, Vec2(associated.box.GetSize().x, 4.0), Vec2(0, (int)associated.box.GetSize().y / 4), SDL_Color {0,0,255,255});
    associated.AddComponent(manabar);
    m_manabar = manabar;
	m_manaRegenTimer.Restart();
}

float ManaSystem::DecreaseMana(float amount)
{
    m_mana -= amount;
    if (auto manabar = m_manabar.lock()) {
        manabar->setAmount(m_mana);
    }
    return amount;
}

float ManaSystem::IncreaseMana()
{
    m_mana += this->m_manaPerIncrease;
    if (auto manabar = m_manabar.lock()) {
        manabar->setAmount(m_mana);
    }
    return m_manaPerIncrease;
}

void ManaSystem::OnEvent(Event& evt)
{
    EventDispatcher dispatcher(evt);
    dispatcher.Dispatch<OnCastSpellEvent>(BIND_EVENT_FN(ManaSystem::OnCastSpell));

}

bool ManaSystem::OnCastSpell(OnCastSpellEvent& evt)
{
    DecreaseMana(evt.GetAmount());
    return true;
}

