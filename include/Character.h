#include "GameObject.h"
#include <memory>
#include <queue>
#include "Timer.h"
class Character: Component{
    public:
        Character (GameObject& associated, std::string sprite);
        ~Character ();
        void Start();
        void Update (float dt);
        void Render ();
        bool Is (std::string type);
        class Command {
            public:
                enum CommandType {MOVE, SHOOT};
                Command (CommandType type, float x, float y);
                CommandType type;
                Vec2 pos;
        };
        void Issue (Command task);
        Character* player;
    private:
        std::weak_ptr<GameObject> gun;
        std::queue<Command> taskQueue;
        Vec2 speed;
        float linearSpeed;
        int hp;
        Timer deathTimer;
};