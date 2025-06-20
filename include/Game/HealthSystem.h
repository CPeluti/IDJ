#include "Core/Component.h"
#include "Core/GameObject.h"
#include "Events/BasicEvent.h"
#include "Core/GameObject.h"
#include "Lifebar.h"
class HealthSystem : public Component, public Observer
{
public:
    HealthSystem(GameObject &associated, float hp);

    void Update(float dt) {}
    void Render() {}
    void OnEvent(Event &evt);

    inline bool Is(std::string type) { return type == "HealthSystem"; }

    float TakeDamage(float damage);
    inline void SetHp(float value) { hp = value; }
    float GetHp() const { return hp; }

    bool OnDamageTaken(OnDamageTakenEvent &evt);

    void SetLifebar(bool enable)
    {
        m_enableLifebar = enable;
        if(auto lifebar = m_lifebar.lock()){
            lifebar->SetEnableLifebar(enable);
        }
    };

private:
    float hp;
    float m_maxHp;
    bool m_enableLifebar = true;
    std::weak_ptr <Lifebar> m_lifebar;
};