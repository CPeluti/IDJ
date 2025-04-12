#pragma once
#include "Core/Component.h" 
#include "Core/Vec2.h" 
#include "Core/Sound.h" 
#include "Core/Timer.h" 
#include <memory> 
class Gun : public Component{
    public:
        Gun(GameObject& associated, std::weak_ptr<GameObject>character);
        void Update(float dt);
        void Render();
        bool Is(std::string type);
        void Shoot(Vec2 target);
    private:
        int projectileAmount;
        Sound shotSound;
        Sound reloadSound;
        int cooldown;
        Timer cdTimer;
        std::weak_ptr<GameObject> character;
        float angle;
        Vec2 bulletOutput;
};