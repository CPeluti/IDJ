#pragma once

#include "GameObject.h"

class Camera
{
public:
    static void Follow(std::weak_ptr<GameObject> newFocus);
    static void Unfollow();
    static void Update(float dt);
    static Vec2 pos;
    static Vec2 speed;
    static float zoom;
    static float smoothness;

private:
    static std::weak_ptr<GameObject> focus;
};