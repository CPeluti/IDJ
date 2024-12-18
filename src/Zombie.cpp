#include "Zombie.h"
#include "GameObject.h"
#include "Animation.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "InputManager.h"
#include "Game.h"
#include "Camera.h"
#include <iostream>

Zombie::Zombie(GameObject &associated) : 
    Component(associated),
    isDead(false), 
    hitpoints(100), 
    damageSound("resources/audio/Hit1.wav"), 
    deathSound("resources/audio/Dead.wav"), 
    hit(false), 
    hitTimer(0.5), 
    deathTimer(5)
{
    SpriteRenderer* srZombie = new SpriteRenderer(associated, "resources/img/Enemy.png", 3, 2);
    associated.AddComponent(srZombie);
    
    Animator* animator = new Animator(associated);
    animator->AddAnimation("walking", new Animation(0, 3, 10));
    animator->AddAnimation("dead", new Animation(5, 5, 0));
    animator->AddAnimation("hit", new Animation(4, 4, 0));
    associated.AddComponent(animator);

    associated.box.x = 600; 
    associated.box.y = 450; 

    animator->SetAnimation("walking");
}

void Zombie::Damage(int dmg){
    Animator* animator = (Animator*)associated.GetComponent("Animator");

    hitpoints -= dmg;

    damageSound.Play(1);
    hit = true;
    hitTimer.Restart();
    animator->SetAnimation("hit");
    if(hitpoints <= 0 && !isDead){
        isDead = true;
        deathSound.Play(1);
        animator->SetAnimation("dead");
    }
}

bool checkClickInsideBox(int x, int y, Rect box){
    return (x > box.x && x < box.x+box.w) && (y > box.y && y < box.y+box.h);
}

void Zombie::Update(float dt){

    // this->Damage(1);
    hitTimer.Update(dt);
    if(isDead){
        
        deathTimer.Update(dt);
        if(deathTimer.Expired()){
            associated.RequestDelete();
        }
    } else {
        InputManager ip = InputManager::GetInstance();
        if(checkClickInsideBox(ip.GetMouseX(), ip.GetMouseY(), associated.box) && ip.MousePress(LEFT_MOUSE_BUTTON)){
            this->Damage(50);
        }
        if(hit && hitTimer.Expired() && !isDead){
            ((Animator*)associated.GetComponent("Animator"))->SetAnimation("walking");  
            hit = false;

        }
    }
    
}

bool Zombie::Is(std::string type){
    return type == "Zombie";
}

void Zombie::Render(){}
