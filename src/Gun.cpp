#include "Gun.h"
#include "SpriteRenderer.h"
#include "Animator.h"

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