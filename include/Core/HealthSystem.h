#include "Component.h"
class HealthSystem: public Component {
    public:
        HealthSystem(GameObject& associated, float hp):Component(associated), hp(hp){}
        
        void Update(float dt){}
        void Render(){}
        void OnEvent(Event& evt){}

        inline bool Is(std::string type){ return type == "HealthSystem"; }

        float TakeDamage(float damage);
        inline void SetHp(float value) {hp = value;}
        float GetHp() const {return hp;}
    private:
        float hp;
};