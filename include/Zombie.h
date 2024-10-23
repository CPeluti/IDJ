#include "Component.h"
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
    int hitpoints;
};