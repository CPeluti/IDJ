#include "Core/Timer.h"

#include "Game/Entity.h"

Entity::Entity() : deathTimer(5)
{
  m_movementSpeed = 0;
  damage = 0;
  isDead = false;
  activeEffects = std::vector<std::weak_ptr<Effect<Entity, float>>>();
  m_Effects = std::vector<std::shared_ptr<Effect<Entity, float>>>();
}

Entity::Entity(float movementSpeed) : deathTimer(5)
{
    this->m_movementSpeed = movementSpeed;
    damage = 0;
    isDead = false;
    activeEffects = std::vector<std::weak_ptr<Effect<Entity, float>>>();
    m_Effects = std::vector<std::shared_ptr<Effect<Entity, float>>>();
}

void Entity::UpdateEffects(float dt)
{
    for (auto it = activeEffects.begin(); it != activeEffects.end();)
    {
        std::shared_ptr<Effect<Entity, float>> e = (*it).lock();
    e->Apply(*this, dt);
    if (e->IsExpired())
    {
      it = activeEffects.erase(it);
    }
    else
    {
      ++it;
    }
  }
}