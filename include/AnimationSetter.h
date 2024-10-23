#include "Component.h"
#include <unordered_map>
class Animation;

class AnimationSetter: public Component {
    public:
        AnimationSetter(GameObject &associated);
        void Update(float dt);
        void Render();
        bool Is(std::string type);
        void SetAnimation(std::string name);
        void AddAnimation(std::string name, Animation *animation);
    private:
        std::unordered_map<std::string, Animation*> animations;
};