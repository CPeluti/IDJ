#pragma once
#include "Component.h"
#include <unordered_map>
class Animation;

class Animator: public Component {
    public:
        Animator(GameObject &associated);
        void Update(float dt);
        void Render();
        bool Is(std::string type);
        void SetAnimation(std::string name);
        void AddAnimation(std::string name, Animation *animation);
    private:
        std::unordered_map<std::string, Animation*> animations;
        int frameStart;
        int frameEnd;
        float frameTime;
        int currentFrame;
        float timeElapsed;
};