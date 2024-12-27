#include <memory>
#include <iostream>
#include "Game.h"
#include "Component.h"
#include "Character.h"
#include "Gun.h"
#include "SpriteRenderer.h"
#include "PlayerController.h"
#include "Animator.h"

Character* Character::player = nullptr;

Character::Character(GameObject& associated, std::string sprite): 
    Component(associated), 
    gun(), 
    taskQueue(), 
    speed{1,1}, 
    linearSpeed(300), 
    hp(100), 
    deathTimer(5)
{
    SpriteRenderer* sr = new SpriteRenderer(associated, sprite, 3, 4);
    Animator* animator = new Animator(associated);
    PlayerController* playerController = new PlayerController(associated);

    associated.AddComponent(sr);
    associated.AddComponent(animator);
    associated.AddComponent(playerController);


    animator->AddAnimation("walking", new Animation(0, 5, 10));
    animator->AddAnimation("idle", new Animation(6, 9, 5));
    animator->AddAnimation("dead", new Animation(10, 11, 5));
    animator->SetAnimation("idle");
}

Character::~Character(){
    player = nullptr;
}

void Character::Start(){
    State& s = Game::GetInstance().GetState();
    GameObject* gunObj = new GameObject(); 
    Gun* gunComponent = new Gun(*gunObj, s.GetObjectPtr(&associated)); 

    gunObj->AddComponent(gunComponent);

    this->gun = s.AddObject(gunObj);

}

void Character::Update(float dt){
    Animator* animator = ((Animator*)associated.GetComponent("Animator"));
    if(hp<=0){
        if(deathTimer.Expired() && !associated.IsDead()){
            associated.RequestDelete();
        } else {
            deathTimer.Update(dt);
            animator->SetAnimation("dead");
        }
        return;
    }
    while(taskQueue.size()>0){
        speed = {0,0};
        Command c = taskQueue.front();
        switch (c.type)
        {
        case c.MOVE:
            speed = c.pos.normalized() * linearSpeed;
            break;
        
        case c.SHOOT:
            if(auto g = gun.lock()){
                ((Gun*)g->GetComponent("Gun"))->Shoot(c.pos);
            }
            break;
        }
        taskQueue.pop();
        if(speed.x || speed.y){
            std::cout << speed.x << std::endl;
            animator->SetAnimation("walking");
            Vec2 newSpeed = (speed * dt);
            associated.box.x += newSpeed.x;
            associated.box.y += newSpeed.y;
        } else {
            animator->SetAnimation("idle");
        }
    }
}

void Character::Render(){}

bool Character::Is(std::string type){
    return type == "Character";
}

void Character::Issue(Command c){
    taskQueue.push(c);
}

Character::Command::Command(CommandType type, Vec2 pos): type(type), pos(pos){}