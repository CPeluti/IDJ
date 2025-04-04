#include "Core/InputManager.h"

#include "Game/PlayerController.h"
#include "Game/Character.h"

PlayerController::PlayerController(GameObject& associated): Component(associated){}

void PlayerController::Start(){}
void PlayerController::Render(){}

void PlayerController::Update(float dt){
    InputManager ip = InputManager::GetInstance();
    Vec2 direction = {0,0};
    if(ip.IsKeyDown(A_KEY)){
        direction.x -= 1;
    } 
    if(ip.IsKeyDown(D_KEY)){
        direction.x += 1;
    }
    if(ip.IsKeyDown(W_KEY)){
        direction.y -= 1;
    }
    if(ip.IsKeyDown(S_KEY)){
        direction.y += 1;
    }
    if(direction.x || direction.y){
        Character::Command c = Character::Command(Character::Command::MOVE, direction);
        ((Character*)associated.GetComponent("Character"))->Issue(c);
    }
    if(ip.MousePress(LEFT_MOUSE_BUTTON)){
        Vec2 target;
        target.x = ip.GetMouseX();
        target.y = ip.GetMouseY();
        Character::Command c = Character::Command(Character::Command::SHOOT, target);
        ((Character*)associated.GetComponent("Character"))->Issue(c);
    }
}

bool PlayerController::Is(std::string type){
    return type == "PlayerController";
}