#include "Game/Spell.h"
#include "Core/Collider.h"

void Spell::castSpell(Vec2 pos, bool targetsPlayer){
    GameObject* go = new GameObject();
    go->box.SetPos(pos);
    SpriteRenderer *sr = new SpriteRenderer(*go, "resources/img/Bullet.png", 1, 1);
    go->AddComponent(sr);
    Collider *collider = new Collider(*go);
    go->AddComponent(collider);

    
    this->targetsPlayer = targetsPlayer;
    this->speed = {speed, .0};
    this->speed = Vec2::Rotate(this->speed, angle);
    this->distanceLeft = maxDistance;
    associated.angleDeg = angle;
    this->damage = damage;
}
