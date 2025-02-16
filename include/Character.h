#include "GameObject.h"
#include <memory>
#include <queue>
#include "Timer.h"
class Character : public Component
{
public:
    Character(GameObject &associated, std::string sprite, bool isPlayer = false);
    ~Character();
    void Start();
    void Update(float dt);
    void Render();
    bool Is(std::string type);
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
    void Issue(Command task);
    static Character *player;
    static int npcCounter;
    void NotifyCollision(GameObject &other);
    Vec2 GetPos();
    void Damage(int amount);
    bool flip;

private:
    std::weak_ptr<GameObject> gun;
    std::queue<Command> taskQueue;
    Vec2 speed;
    float linearSpeed;
    int hp;
    bool isDead;
    Timer deathTimer;
};