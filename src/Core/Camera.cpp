#include "Core/Camera.h"
#include "Core/InputManager.h"
#include "Core/Game.h"
#include "Core/Log.h"
#include <iostream>
Vec2 Camera::pos = {0, 0};
Vec2 Camera::speed = {0, 0};
float Camera::zoom = 1.0f;
float Camera::smoothness = 0.0f;

std::weak_ptr<GameObject> Camera::focus = std::weak_ptr<GameObject>();

void Camera::Follow(std::weak_ptr<GameObject> newFocus)
{
    focus = newFocus;
}

void Camera::Unfollow()
{
    focus.reset();
}

void Camera::Update(float dt)
{
    const int baseSpeed = 600;
    InputManager ip = InputManager::GetInstance();
    Vec2 direction = {0, 0};
    Game &game = Game::GetInstance();

    if (auto focusLocked = focus.lock())
    {
        Vec2 size = game.GetWindowSize();

        Vec2 pos = focusLocked->box.center();
		Vec2 target = { pos.x * Camera::zoom - (size.x / 2) , pos.y * Camera::zoom - (size.y / 2) };

        float weight = (11.f-smoothness) / 100;

        Camera::pos.x = Vec2::lerp(Camera::pos.x, target.x, weight);
        Camera::pos.y = Vec2::lerp(Camera::pos.y, target.y, weight);


    }
    else
    {
        if (ip.IsKeyDown(LEFT_ARROW_KEY))
        {
            direction.x -= 1;
        }
        if (ip.IsKeyDown(RIGHT_ARROW_KEY))
        {
            direction.x += 1;
        }
        if (ip.IsKeyDown(UP_ARROW_KEY))
        {
            direction.y -= 1;
        }
        if (ip.IsKeyDown(DOWN_ARROW_KEY))
        {
            direction.y += 1;
        }
        if (direction != Vec2::Zero)
        {
            Camera::speed = direction.normalized() * baseSpeed;
            Camera::pos = Camera::pos + (Camera::speed * dt);
        }
    }

    Camera::pos.x = floor(Camera::pos.x);
    Camera::pos.y = floor(Camera::pos.y);
}