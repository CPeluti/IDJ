#include "Component.h"
#include "Sound.h"
#include "Timer.h"
class GameObject;
class Zombie : public Component
{
public:
    Zombie(GameObject &associated);
    void Damage(int dmg);
    void Update(float dt);
    void Render();
    bool Is(std::string type);

private:
    bool isDead;
    int hitpoints;
    Sound damageSound;
    Sound deathSound;
    bool hit;
    
    Timer hitTimer;
    Timer deathTimer;
    
};