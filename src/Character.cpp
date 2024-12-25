#include <memory>
#include "Game.h"
#include "Component.h"
#include "Character.h"
#include "Gun.h"
#include "SpriteRenderer.h"
#include "Animator.h"

Character* Character::player = nullptr;

Character::Character(GameObject& associated, std::string sprite): 
    Component(associated), 
    gun(), 
    taskQueue(), 
    speed{1,1}, 
    linearSpeed(1), 
    hp(100), 
    deathTimer(5)
{
    SpriteRenderer* sr = new SpriteRenderer(associated, sprite, 3, 4);
    Animator* animator = new Animator(associated);

    associated.AddComponent(sr);
    associated.AddComponent(animator);

    animator->AddAnimation("walking", new Animation(0, 5, 10));
    animator->AddAnimation("idle", new Animation(6, 9, 5));
    animator->AddAnimation("dead", new Animation(10, 11, 5));
}

Character::~Character(){
    player = nullptr;
}

void Character::Start(){

    GameObject* gun = new GameObject(); 
    Gun* gunComponent = new Gun(*gun, std::weak_ptr<GameObject>(std::shared_ptr<GameObject>(&associated))); 

    gun->AddComponent(gunComponent);

    this->gun = std::weak_ptr(std::shared_ptr<GameObject>(gun));
    Game::GetInstance().GetState().AddObject(gun);
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
            animator->SetAnimation("walking");
        } else {
            animator->SetAnimation("idle");
        }
    }
}