#include "Core/Component.h"
#include "Core/GameObject.h"
#include "Events/BasicEvent.h"
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
    inline void SetMana(float value) { m_mana = value; }
    inline float GetMana() const { return m_mana; }


    void SetManabar(bool enable)
    {
        m_enableManabar = enable;
        if(auto manabar = m_manabar.lock()){
            manabar->SetEnableLifebar(enable);
        }
    };
    bool OnCastSpell(OnCastSpellEvent& evt);
private:
    float m_mana;
    float m_maxMana;
    bool m_enableManabar= true;
    std::weak_ptr <Lifebar> m_manabar;
};