#include "Gun.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include <iostream>
#define offset 100
Gun::Gun(GameObject& associated, std::weak_ptr<GameObject>character): 
Component(associated), 
shotSound("resources/audio/Range.wav"),
reloadSound("resources/audio/PumpAction.mp3"),
cooldown(1),
cdTimer(cooldown),
character(character),
angle(0)
{
    SpriteRenderer* sr = new SpriteRenderer(associated, "resources/img/Gun.png", 3, 2);
    Animator* animator = new Animator(associated);
    
    associated.AddComponent(sr);
    associated.AddComponent(animator);
    animator->AddAnimation("idle", new Animation(0, 0, 0));
    animator->AddAnimation("reloading", new Animation(1, 5, cooldown/5.0));

    animator->SetAnimation("idle");
}

void Gun::Update(float dt){
    Animator* animator = (Animator*)associated.GetComponent("Animator");
    if(auto c = character.lock()){
        int centroCharX = c->box.x+c->box.w/2; 
        int centroCharY = c->box.y+c->box.h/2;

        associated.box.x = centroCharX - associated.box.w/2;
        associated.box.y = centroCharY - associated.box.h/2;
        if(angle > 90 && angle <=270){
            associated.box.x -= offset;
        } else {
            associated.box.x += offset;
        }
        if(cdTimer.Expired()){
            animator->SetAnimation("idle");

        } else {
            animator->SetAnimation("reloading");
            if(cdTimer.GetTime() == 0){
                reloadSound.Play();
            }
            cdTimer.Update(dt);
        }
    } else {
        associated.RequestDelete();
        return;
    }
}

void Gun::Shoot(Vec2 target){
    Vec2 centro;
    if(auto c = character.lock()){
        centro.x = c->box.x+c->box.w/2; 
        centro.y = c->box.y+c->box.h/2;
    }
    if(cdTimer.Expired()){
        angle = Vec2::Angle(centro,target);
        std::cout << angle << std::endl;
        shotSound.Play();
        cdTimer.Restart();
    }
}

bool Gun::Is(std::string type){
    return type == "Gun";
}

void Gun::Render(){}