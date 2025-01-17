#include "Bullet.h"
#include "SpriteRenderer.h"
#include "Vec2.h"
Bullet::Bullet(GameObject& associated, float angle, float speed, int damage, float maxDistance):Component(associated){
    SpriteRenderer* sr = new SpriteRenderer(associated, "resources/img/Bullet.png", 1, 1);
    associated.AddComponent(sr);

    this->speed = {speed, .0};
    this->speed = Vec2::Rotate(this->speed, angle);
    this->distanceLeft = maxDistance;
    associated.angleDeg = angle;
    // this->speed = {speed, speed}
}

void Bullet::Update(float dt){
    
    Vec2 oldPos = associated.box.GetPos();
    Vec2 newPos = {
        oldPos.x + (speed.x * dt),
        oldPos.y + (speed.y * dt)
    };

    associated.box.RawMove(newPos);

    distanceLeft -= Vec2::Distance(newPos, oldPos);

    if(distanceLeft <= 0){
        associated.RequestDelete();
    }
}

bool Bullet::Is(std::string type){
    return type == "Bullet";
}

int Bullet::GetDamage(){
    return damage;
}

void Bullet::Render(){}