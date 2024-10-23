#include "Zombie.h"
#include "GameObject.h"
#include "Animation.h"
#include "SpriteRenderer.h"
#include "AnimationSetter.h"

Zombie::Zombie(GameObject &associated) : Component(associated), hitpoints(100){
    SpriteRenderer* srZombie = new SpriteRenderer(associated, "resources/img/Enemy.png", 3, 2);
    associated.AddComponent(srZombie);
    
    AnimationSetter* animationSetter = new AnimationSetter(associated);
    animationSetter->AddAnimation("walking", new Animation(0, 3, 10));
    animationSetter->AddAnimation("dead", new Animation(5, 5, 0));
    associated.AddComponent(animationSetter);

    associated.box.x = 600; 
    associated.box.y = 450; 

    animationSetter->SetAnimation("walking");
}

void Zombie::Damage(int dmg){
    hitpoints -= dmg;
    if(hitpoints <= 0){
        AnimationSetter* as = (AnimationSetter*)associated.GetComponent("AnimationSetter") ;
        as->SetAnimation("dead");
    }
}

void Zombie::Update(float dt){
    this->Damage(1);
}

bool Zombie::Is(std::string type){
    return type == "Zombie";
}

void Zombie::Render(){}
