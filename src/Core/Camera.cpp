#include "Core/Camera.h"
#include "Core/InputManager.h"
#include "Core/Game.h"
#include <iostream>
Vec2 Camera::pos = {0,0};
Vec2 Camera::speed = {0,0};

GameObject* Camera::focus = nullptr;

void Camera::Follow(GameObject* newFocus){
    focus = newFocus;
}

void Camera::Unfollow(){
    focus = nullptr;
}

void Camera::Update(float dt){
    const int baseSpeed = 200; 
    InputManager ip = InputManager::GetInstance();
    Vec2 direction = {0,0};
    Game& game = Game::GetInstance();

    if(focus != nullptr){
        Vec2 size = game.GetWindowSize();

        Vec2 pos = focus->box.GetPos(); 
        Camera::pos.x = pos.x-(size.x/2);
        Camera::pos.y = pos.y-(size.y/2);
    } else {
        if(ip.IsKeyDown(LEFT_ARROW_KEY)){
            direction.x -= 1;
        } 
        if(ip.IsKeyDown(RIGHT_ARROW_KEY)){
            direction.x += 1;
        }
        if(ip.IsKeyDown(UP_ARROW_KEY)){
            direction.y -= 1;
        }
        if(ip.IsKeyDown(DOWN_ARROW_KEY)){
            direction.y += 1;
        }
        if(direction != Vec2::Zero){
            Camera::speed = direction.normalized() * baseSpeed;
            Camera::pos = Camera::pos + (Camera::speed*dt);
        }
    }
}