#include <memory>
#include <iostream>
#include "Game.h"
#include "Component.h"
#include "Character.h"
#include "Camera.h"
#include "Bullet.h"
#include "Zombie.h"
#include "Gun.h"
#include "SpriteRenderer.h"
#include "PlayerController.h"
#include "Animator.h"
#include "Collider.h"

Character *Character::player = nullptr;
int Character::npcCounter = 0;

Character::Character(GameObject &associated, std::string sprite, bool isPlayer) : Component(associated),
                                                                   gun(),
                                                                   taskQueue(),
                                                                   speed{1, 1},
                                                                   linearSpeed(300),
                                                                   hp(500),
                                                                   deathTimer(5),
                                                                   isDead(false)
{
    SpriteRenderer *sr = new SpriteRenderer(associated, sprite, 3, 4);
    Animator *animator = new Animator(associated);
    if(!isPlayer){
        Character::npcCounter++;
    } else {
        PlayerController *playerController = new PlayerController(associated);
        associated.AddComponent(playerController);
    }
    Collider *collider = new Collider(associated);

    associated.AddComponent(sr);
    associated.AddComponent(animator);
    associated.AddComponent(collider);

    animator->AddAnimation("walking", new Animation(0, 5, 0.2));
    animator->AddAnimation("idle", new Animation(6, 9, 0.5));
    animator->AddAnimation("i_walking", new Animation(0, 5, 0.2, SDL_FLIP_HORIZONTAL));
    animator->AddAnimation("i_idle", new Animation(6, 9, 0.5, SDL_FLIP_HORIZONTAL));
    animator->AddAnimation("dead", new Animation(10, 11, 0.5));
    animator->SetAnimation("idle");
    flip = false;

}

Character::~Character()
{
    if(Character::player != this){
        Character::npcCounter--;
    } else {
        Character::player = nullptr;
    }
}

void Character::Start()
{
    State &s = Game::GetInstance().GetState();
    GameObject *gunObj = new GameObject();
    Gun *gunComponent = new Gun(*gunObj, s.GetObjectPtr(&associated));

    gunObj->AddComponent(gunComponent);

    this->gun = s.AddObject(gunObj);
}
void Character::Damage(int amount){
    hp -= amount;
    Animator *animator = (Animator *)associated.GetComponent("Animator");
    animator->SetAnimation("hit");
    if (hp <= 0 && !isDead)
    {
        if(auto g = this->gun.lock()){
            g->RequestDelete();
        }
        isDead = true;
        deathTimer.Restart();
        animator->SetAnimation("dead");
        if(Character::player == this){
            Camera::Unfollow();
        }
    }
}
void Character::Update(float dt)
{
    Animator *animator = ((Animator *)associated.GetComponent("Animator"));
    if (isDead)
    {
        deathTimer.Update(dt);
        if (deathTimer.Expired())
        {
            associated.RequestDelete();
        }
        return;
    }
    if (taskQueue.size() == 0)
    {
        animator->SetAnimation(flip ? "idle" : "i_idle");
    }
    while (taskQueue.size() > 0)
    {
        speed = {0, 0};
        Command c = taskQueue.front();
        switch (c.type)
        {
        case c.MOVE:
            speed = c.pos.normalized() * linearSpeed;
            break;

        case c.SHOOT:
            if (auto g = gun.lock())
            {
                ((Gun *)g->GetComponent("Gun"))->Shoot(c.pos);
            }
            break;
        }
        taskQueue.pop();
        if (speed.x || speed.y)
        {
            animator->SetAnimation(flip ? "walking" : "i_walking");
            Vec2 newSpeed = (speed * dt);
            Vec2 currentPos = associated.box.GetPos();

            associated.box.RawMove(currentPos + newSpeed);
        }
    }
}

void Character::Render() {}

bool Character::Is(std::string type)
{
    return type == "Character";
}

void Character::Issue(Command c)
{
    taskQueue.push(c);
}

Character::Command::Command(CommandType type, Vec2 pos) : type(type), pos(pos) {}

void Character::NotifyCollision(GameObject &other)
{
    Bullet *b = (Bullet *)other.GetComponent("Bullet");
    Zombie *z = (Zombie *)other.GetComponent("Zombie");
    if (b != nullptr && ((Character::player == this && b->targetsPlayer) || (Character::player != this)))
    {
        Damage(b->GetDamage());
    }
    if (z != nullptr && !z->isDead && (Character::player == this))
    {
        Damage(z->GetDamage());
    }
}

Vec2 Character::GetPos()
{
    return this->associated.box.center();
}