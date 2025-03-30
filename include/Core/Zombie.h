#include "Component.h"
#include "Sound.h"
#include "Timer.h"
#include "GameObject.h"
class Zombie : public Component, public Observer
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
    bool isDead;
    bool flip;
    void OnEvent(Event& e);
private:
    bool OnCollision(OnCollisionEvent& evt);

private:
    int hitpoints;
    Sound damageSound;
    Sound deathSound;
    bool hit;
    int damage;
    Timer hitTimer;
    Timer deathTimer;
};