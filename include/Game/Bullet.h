#include "Core/GameObject.h"
#include "Core/ParticleSystem.h"

#include <string>
#include <memory>
class Bullet : public Component, public Observer
{
public:
    Bullet(GameObject &associated, float angle, float speed, int damage, float maxDistance, bool targetsPlayer);
    void Update(float dt);
    void Render();
    void Start();
    bool Is(std::string type);
    int GetDamage();
    void OnEvent(Event& evt);
    bool targetsPlayer;

private:
    bool OnCollision(OnCollisionEvent& evt);

private:
    Vec2 speed;
    float distanceLeft;
    int damage;
    std::weak_ptr<GameObject> particlesSystem;
    ParticleData m_Particle;
};