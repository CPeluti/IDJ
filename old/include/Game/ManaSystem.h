#pragma once
#include "Core/Component.h"
#include "Core/GameObject.h"
#include "Core/Timer.h"
#include "Core/GameObject.h"
#include "Lifebar.h"
class ManaSystem : public Component, public Observer
{
public:
    ManaSystem(GameObject &associated, float mana);

    void Update(float dt) {}
    void Render() {}
    void OnEvent(Event &evt);

    inline bool Is(std::string type) { return type == "ManaSystem"; }

    float DecreaseMana(float amount);
    float IncreaseMana();
    inline void SetMana(float value) { m_mana = value; }
    inline float GetMana() const { return m_mana; }
	inline bool isManaFull() const { return m_mana >= m_maxMana; }

    void SetManabar(bool enable)
    {
        m_enableManabar = enable;
        if(auto manabar = m_manabar.lock()){
            manabar->SetEnableLifebar(enable);
        }
    };
    bool OnCastSpell(OnCastSpellEvent& evt);
	Timer m_manaRegenTimer;
private:
    float m_mana;
    float m_maxMana;
    float m_manaPerIncrease;
    bool m_enableManabar= true;
    std::weak_ptr <Lifebar> m_manabar;
};