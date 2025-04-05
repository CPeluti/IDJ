#include "Core/Component.h"
#include "Core/Sound.h"
#include "Core/Timer.h"
#include "Core/GameObject.h"
#include "Entity.h"
class Zombie : public Component, public Observer, public Entity
{
public:
    Zombie(GameObject &associated);
    ~Zombie();
    void Start();
    void Damage(int dmg);
    void Update(float dt);
    void Render();
    bool Is(std::string type);
    int GetDamage();
    static int zombieCounter;
    bool flip;
    void OnEvent(Event &e);

private:
    bool OnCollision(OnCollisionEvent &evt);
    bool OnDamageTaken(OnDamageTakenEvent &evt);

private:
    int hitpoints;
    Sound damageSound;
    Sound deathSound;
    bool hit;
    bool isDead;
    int damage;
    Timer hitTimer;
    Timer deathTimer;
};