#include "Zombie.h"
#include "GameObject.h"
#include "Animation.h"
#include "SpriteRenderer.h"
#include "Bullet.h"
#include "Animator.h"
#include "Character.h"
#include "InputManager.h"
#include "Game.h"
#include "Camera.h"
#include "Collider.h"
#include <iostream>

int Zombie::zombieCounter = 0;

Zombie::Zombie(GameObject &associated) : Component(associated),
                                         isDead(false),
                                         hitpoints(600),
                                         damageSound("resources/audio/Hit1.wav"),
                                         deathSound("resources/audio/Dead.wav"),
                                         hit(false),
                                         hitTimer(0.5),
                                         damage(50),
                                         deathTimer(5)
{
    SpriteRenderer *srZombie = new SpriteRenderer(associated, "resources/img/Enemy.png", 3, 2);
    associated.AddComponent(srZombie);
    Collider *collider = new Collider(associated);
    associated.AddComponent(collider);

    Animator *animator = new Animator(associated);
    animator->AddAnimation("walking", new Animation(0, 3, 0.3));
    animator->AddAnimation("r_walking", new Animation(0, 3, 0.3, SDL_FLIP_HORIZONTAL));
    animator->AddAnimation("dead", new Animation(5, 5, 0));
    animator->AddAnimation("hit", new Animation(4, 4, 0));
    animator->AddAnimation("r_hit", new Animation(4, 4, 0, SDL_FLIP_HORIZONTAL));
    associated.AddComponent(animator);

    associated.box.Move({600, 450});

    animator->SetAnimation("walking");
    zombieCounter++;
}

void Zombie::Damage(int dmg)
{
    Animator *animator = (Animator *)associated.GetComponent("Animator");

    hitpoints -= dmg;

    damageSound.Play(1);
    hit = true;
    hitTimer.Restart();
    if(flip) animator->SetAnimation("r_hit");
    else animator->SetAnimation("hit");
    if (hitpoints <= 0 && !isDead)
    {
        this->associated.RemoveComponent(this->associated.GetComponent("Collider"));
        isDead = true;
        deathTimer.Restart();
        deathSound.Play(1);
        animator->SetAnimation("dead");
    }
}

bool checkClickInsideBox(int x, int y, float boxX, float boxY, float boxW, float boxH)
{
    return (x > boxX && x < boxX + boxW) && (y > boxY && y < boxY + boxH);
}

Zombie::~Zombie()
{
    zombieCounter--;
}
void Zombie::Start() {}

void Zombie::Update(float dt)
{
    Animator *animator = ((Animator *)associated.GetComponent("Animator"));
    // this->Damage(1);
    hitTimer.Update(dt);
    if (isDead)
    {

        deathTimer.Update(dt);
        if (deathTimer.Expired())
        {
            associated.RequestDelete();
        }
    }
    else
    {
        if (!hit && Character::player != nullptr)
        {
            Vec2 playerPos = Character::player->GetPos();
            Vec2 distance = playerPos - associated.box.center();
            Vec2 currentPos = this->associated.box.center();
            this->associated.box.Move(currentPos + distance * dt * 0.5);
            flip = distance.x < 0;
            if(flip){
                animator->SetAnimation("r_walking");
            } else {
                animator->SetAnimation("walking");
            }
        }
        if (hit && hitTimer.Expired() && !isDead)
        {
            if(flip){
                animator->SetAnimation("r_walking");
            } else {
                animator->SetAnimation("walking");
            }
            
            hit = false;
        }
    }
}

bool Zombie::Is(std::string type)
{
    return type == "Zombie";
}

void Zombie::Render() {}

int Zombie::GetDamage()
{
    return damage;
}

void Zombie::NotifyCollision(GameObject &other)
{
    Bullet *b = (Bullet *)other.GetComponent("Bullet");
    if (b != nullptr && !this->isDead)
    {
        this->Damage(b->GetDamage());
    }
}
