#include "Component.h"
#include "Observer.h"
#include "GameObject.h"

class DamageComponent : public Component, public Observer {


    DamageComponent(GameObject &associated);
    ~DamageComponent();
    void Start();
    void Update(float dt);
    void Render();

    void onNotify(Component& comp, Observer::Event evt){
        switch (evt)
        {
        case Observer::Event::onCollide:
            // this->associated.subject.notify(this, Observer::Event::onTakeDamage, []);
            break;
        
        default:
            break;
        }
    }

    // void NotifyCollision(GameObject &other);

    bool Is(std::string type);

};