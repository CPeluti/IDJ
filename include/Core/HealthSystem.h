#include "Component.h"
#include "Events/BasicEvent.h"
#include "GameObject.h"
class HealthSystem: public Component, public Observer {
    public:
        HealthSystem(GameObject& associated, float hp);
        
        void Update(float dt){}
        void Render(){}
        void OnEvent(Event& evt);

        inline bool Is(std::string type){ return type == "HealthSystem"; }

        float TakeDamage(float damage);
        inline void SetHp(float value) {hp = value;}
        float GetHp() const {return hp;}


        bool OnDamageTaken(OnDamageTakenEvent& evt);
    private:
        float hp;
};