#include "Core/GameObject.h"
#include "Core/Subject.h"
#include "Core/Timer.h"

#include "Entity.h"

#include <memory>
#include <queue>
class Character : public Component, public Observer, public Entity
{
public:
    class Command
    {
    public:
        enum CommandType
        {
            MOVE,
            SHOOT
        };
        Command(CommandType type, Vec2 pos);
        CommandType type;
        Vec2 pos;
    };
    Character(GameObject &associated, std::string sprite, bool isPlayer = false);
    ~Character();
    void Start();
    void Update(float dt);
    void Render();
    inline void Issue(Command task) { taskQueue.push(task); }

    // void Damage(int amount);

    // void OnNotify(Component& component);
    void OnEvent(Event &evt);

    inline Vec2 GetPos() const { return associated.box.center(); }
    inline int getHP() const { return hp; }
    inline int getProjectileNumber() const { return extraProjectiles; }

    inline void SetFlip(bool value) { flip = value; }

    inline bool Is(std::string type) { return type == "Character"; }
    // COMPONENT_IS("Character");

public:
    static int npcCounter;
    static Character *player;

private:
    bool OnCollision(OnCollisionEvent &evt);

private:
    bool flip;
    Subject subject;
    std::weak_ptr<GameObject> gun;
    std::queue<Command> taskQueue;
    Vec2 speed;
    float hp;
    bool isDead;
    Timer deathTimer;
    int extraProjectiles;
};