#include "Gun.h"
#include "SpriteRenderer.h"
#include "Animator.h"
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
    animator->AddAnimation("reloading", new Animation(1, 5, 5));
}

void Gun::Update(float dt){
    Animator* animator = new Animator(associated);
    if(auto c = character.lock()){
        if(cdTimer.Expired()){
            int centroCharX = c->box.x+c->box.w/2; 
            int centroCharY = c->box.y+c->box.h/2;

            associated.box.x = centroCharX - associated.box.w/2;
            associated.box.y = centroCharY - associated.box.h/2;

            if(angle > 180 && angle <=360){
                associated.box.x -= offset;
            } else {
                associated.box.x += offset;
            }
        } else {
            animator->SetAnimation("reloading");
            reloadSound.Play();
            cdTimer.Update(dt);
        }
    } else {
        associated.RequestDelete();
        return;
    }
}

void Gun::Shoot(Vec2 target){
    if(cdTimer.Expired()){
        Vec2 centro = {associated.box.x+associated.box.w/2, associated.box.y+associated.box.h/2};
        Vec2::Angle(centro,target);
        shotSound.Play();
        cdTimer.Restart();
    }
}

bool Gun::Is(std::string type){
    return type == "Gun";
}