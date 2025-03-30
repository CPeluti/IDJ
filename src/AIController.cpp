#include "Core/AIController.h"
#include "Core/Character.h"
#include <iostream>

AIController::AIController(GameObject& associated) : Component(associated), restTimer(3){
    state = RESTING;
}

void AIController::Render(){}

void AIController::Update(float dt){
    switch (state)
    {
    case RESTING:
        /* code */
        restTimer.Update(dt);
        if(restTimer.Expired()){
            state = MOVING;
        }
        break;
    
    case MOVING:
            if(Character::player != nullptr){
                Vec2 playerPos = Character::player->GetPos();
                Vec2 distance = (playerPos - associated.box.center()).normalized();
                ((Character*)this->associated.GetComponent("Character"))->Issue(Character::Command(Character::Command::MOVE,distance));
                if(Vec2::Distance(playerPos,this->associated.box.GetPos()) < 400){
                    ((Character*)this->associated.GetComponent("Character"))->Issue(Character::Command(Character::Command::SHOOT,playerPos));
                    restTimer.Restart();
                    state=RESTING;
                }
            }
        break;
    }
}

bool AIController::Is(std::string type){
    return type == "AIController";
}