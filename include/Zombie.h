#include "Component.h"
#include "Sound.h"
#include "Timer.h"
#include "GameObject.h"
class Zombie : public Component
{
public:
    Zombie(GameObject &associated);
    ~Zombie();
    void Start();
    void Damage(int dmg);
    void Update(float dt);
    void Render();
    bool Is(std::string type);
    void NotifyCollision(GameObject &other);
    int GetDamage();
    static int zombieCounter;
    bool isDead;
    bool flip;

private:
    int hitpoints;
    Sound damageSound;
    Sound deathSound;
    bool hit;
    int damage;
    Timer hitTimer;
    Timer deathTimer;
};