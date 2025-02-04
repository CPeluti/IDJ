#include "Bullet.h"
#include "SpriteRenderer.h"
#include "Vec2.h"
#include "Collider.h"

#include <iostream>

Bullet::Bullet(GameObject &associated, float angle, float speed, int damage, float maxDistance, bool targetsPlayer) : Component(associated)
{
    SpriteRenderer *sr = new SpriteRenderer(associated, "resources/img/Bullet.png", 1, 1);
    associated.AddComponent(sr);
    Collider *collider = new Collider(associated);
    associated.AddComponent(collider);

    this->targetsPlayer = targetsPlayer;
    this->speed = {speed, .0};
    this->speed = Vec2::Rotate(this->speed, angle);
    this->distanceLeft = maxDistance;
    associated.angleDeg = angle;
    this->damage = damage;
    // this->speed = {speed, speed}
}

void Bullet::Update(float dt)
{

    Vec2 oldPos = associated.box.GetPos();
    Vec2 newPos = {
        oldPos.x + (speed.x * dt),
        oldPos.y + (speed.y * dt)};

    associated.box.RawMove(newPos);

    distanceLeft -= Vec2::Distance(newPos, oldPos);

    if (distanceLeft <= 0)
    {
        associated.RequestDelete();
    }
}

bool Bullet::Is(std::string type)
{
    return type == "Bullet";
}

int Bullet::GetDamage()
{
    return damage;
}

void Bullet::Render() {}

void Bullet::NotifyCollision(GameObject &other)
{
    if (other.GetComponent("Zombie") != nullptr || (other.GetComponent("Character") != nullptr))
    {
        this->associated.RequestDelete();
    }
}