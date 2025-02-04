#include "Vec2.h"
#include "Timer.h"
#include "Component.h"
class AIController : public Component{
    public:
        AIController(GameObject& associated);
        void Update(float dt);
        void Render();
        bool Is(std::string type);
    private:
        enum AIState {
            MOVING,
            RESTING
        };
        AIState state;
        Timer restTimer;
        Vec2 Destination;
};