#include "Core/Component.h"
#include "Core/Sound.h"
#include "Core/Timer.h"
#include "Core/GameObject.h"
#include "Entity.h"
class Dummy : public Component, public Observer, public Entity, public std::enable_shared_from_this<Dummy>
{
public:
    Dummy(GameObject &associated);
    ~Dummy();
    void Start();
    void Damage(int dmg);
    void Update(float dt);
    void Render();
    bool Is(std::string type);
    int GetDamage();
    static int dummyCounter;
    bool flip;
    void OnEvent(Event &e);

private:
    bool OnCollision(OnCollisionEvent &evt);
    bool OnDamageTaken(OnDamageTakenEvent &evt);
    bool OnInteraction(OnInteractionEvent &evt);

private:
    std::weak_ptr<GameObject> exclamation;
    bool playerFound = false;
    bool moving = false;
    Vec2 characterBreadcrumb;
    int hitpoints;
    Sound damageSound;
    Sound deathSound;
    bool hit;
    bool isDead;
    int damage;
    Timer hitTimer;
    Timer deathTimer;
};