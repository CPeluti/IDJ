#include "Core/Bullet.h"
#include "Core/SpriteRenderer.h"
#include "Core/Vec2.h"
#include "Core/Collider.h"

#include <iostream>

Bullet::Bullet(GameObject &associated, float angle, float speed, int damage, float maxDistance, bool targetsPlayer) : Component(associated)
{
    this->associated.subject.addObserver(this);
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

void Bullet::OnEvent(Event& evt){
    EventDispatcher dispatcher(evt);

    dispatcher.Dispatch<OnCollisionEvent>(BIND_EVENT_FN(Bullet::OnCollision));
}

bool Bullet::OnCollision(OnCollisionEvent& evt)
{
    GameObject& go = evt.GetGameObject();

    if (go.GetComponent("Zombie") != nullptr || (go.GetComponent("Character") != nullptr))
    {
        this->associated.RequestDelete();
    }
    return true;
}