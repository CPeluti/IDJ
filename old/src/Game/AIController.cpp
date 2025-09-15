#include "Game/AIController.h"
#include "Game/Character.h"
#include "Core/Component.h"
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
            if(auto character = Character::player.lock()){
                Vec2 playerPos = character->GetPos();
                Vec2 distance = (playerPos - associated.box.center()).normalized();
                if(auto character = std::dynamic_pointer_cast<Character>(this->associated.GetComponent("Character").lock())){
                    character->Issue(Character::Command(Character::Command::MOVE,distance));
                    if(Vec2::Distance(playerPos,this->associated.box.GetPos()) < 400){
                        character->Issue(Character::Command(Character::Command::SHOOT,playerPos));
                        restTimer.Restart();
                        state=RESTING;
                    }
                }
            }
        break;
    }
}

bool AIController::Is(std::string type){
    return type == "AIController";
}
