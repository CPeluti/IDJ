#include "Gun.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "InputManager.h"
#include "Character.h"
#include "Game.h"
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
    animator->AddAnimation("i_idle", new Animation(0, 0, 0, SDL_FLIP_VERTICAL));
    animator->AddAnimation("i_reloading", new Animation(1, 5, cooldown/5.0,SDL_FLIP_VERTICAL));

    animator->SetAnimation("idle");
}

void Gun::Update(float dt){
    Animator* animator = (Animator*)associated.GetComponent("Animator");
    InputManager& ip = InputManager::GetInstance();

    bool inverted = true;
    if(auto c = character.lock()){

        float centroCharX = c->box.x+c->box.w/2; 
        float centroCharY = c->box.y+c->box.h/2;
        Vec2 centro = {centroCharX, centroCharY};
        Vec2 target = {(float)ip.GetMouseX(), (float)ip.GetMouseY()};
        
        angle = Vec2::Angle(centro,target);
        associated.angleDeg = angle;

        if(angle >= 90 && angle <=270){
            inverted = false;
            Character::player->flip = false;
        } else {
            inverted = true;
            Character::player->flip = true;
        }

        associated.box.x = centroCharX - associated.box.w/2;
        associated.box.y = centroCharY - associated.box.h/2;
        
        Vec2 pos = {associated.box.w, 0};
        double rad = (double)angle*M_PI/180;
        pos = Vec2::Rotate(pos, rad);
        associated.box.x += pos.x;
        associated.box.y += pos.y;

        if(cdTimer.Expired()){
            animator->SetAnimation(inverted?"idle":"i_idle");
        } else {
            animator->SetAnimation(inverted?"reloading":"i_reloading");
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
        // Vec2 pos = {associated.box.x, associated.box.y};
        // Vec2 teste = Vec2::Rotate(targe)
        associated.angleDeg = angle;
        shotSound.Play();
        cdTimer.Restart();
    }
}

bool Gun::Is(std::string type){
    return type == "Gun";
}

void Gun::Render(){}