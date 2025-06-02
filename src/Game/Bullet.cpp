#include "Core/SpriteRenderer.h"
#include "Core/Vec2.h"
#include "Core/Collider.h"
#include "Game/Bullet.h"

#include <iostream>
#include <vector>

Bullet::Bullet(GameObject &associated, float angle, float speed, int damage, float maxDistance, bool targetsPlayer) : Component(associated)
{
    this->associated.subject.addObserver(this);
    // ParticleSystem* ps = new ParticleSystem(associated);
    // associated.AddComponent(ps);
    SpriteRenderer *sr = new SpriteRenderer(associated, "resources/img/Bullet.png", 1, 1);
    associated.AddComponent(sr);
    std::vector<std::string> layers;
    layers.push_back("layer1");
    Collider *collider = new Collider(associated, layers, new OnCollisionEvent(associated));
    associated.AddComponent(collider);

    this->targetsPlayer = targetsPlayer;
    this->speed = {speed, .0};
    this->speed = Vec2::Rotate(this->speed, angle);
    this->distanceLeft = maxDistance;
    associated.angleDeg = angle;
    this->damage = damage;
    // this->speed = {speed, speed}
}

void Bullet::Start()
{
    m_Particle.SizeBegin = 4.0f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 1.0f;
    m_Particle.LifeTime = 1.0f;
    m_Particle.Velocity = {0.0f, 0.0f};
    m_Particle.VelocityVariation = {3.0f, 1.0f};
    m_Particle.Position = {0.0f, 0.0f};
}

void Bullet::Update(float dt)
{

    Vec2 oldPos = associated.box.GetPos();
    Vec2 newPos = {
        oldPos.x + (speed.x * dt),
        oldPos.y + (speed.y * dt)};

    associated.box.RawMove(newPos);

    distanceLeft -= Vec2::Distance(newPos, oldPos);
    ParticleSystem *particles = (ParticleSystem *)associated.GetComponent("ParticleSystem");
    if (particles)
    {
        m_Particle.Position = associated.box.center();
        for (int i = 0; i < 1; i++)
        {
            particles->Emit(m_Particle);
        }
    }

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

void Bullet::OnEvent(Event &evt)
{
    EventDispatcher dispatcher(evt);

    dispatcher.Dispatch<OnCollisionEvent>(BIND_EVENT_FN(Bullet::OnCollision));
}

bool Bullet::OnCollision(OnCollisionEvent &evt)
{
    GameObject &go = evt.GetGameObject();
    OnDamageTakenEvent e = OnDamageTakenEvent(this->associated, this->damage);
    if (go.GetComponent("HealthSystem"))
        go.subject.notify(e);
    if (!go.GetComponent("Bullet"))
        this->associated.RequestDelete();

    return true;
}