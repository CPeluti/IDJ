#include "Core/Timer.h"

#include "Game/Entity.h"

std::vector<std::weak_ptr<Entity>> Entity::m_enemies = std::vector<std::weak_ptr<Entity>>();

Entity::Entity() : deathTimer(5)
{
  m_movementSpeed = 0;
  damage = 0;
  isDead = false;
  activeEffects = std::vector<Effect<Entity>*>();
  m_Effects = std::vector<Effect<Entity>*>();
}

Entity::Entity(float movementSpeed) : deathTimer(5)
{
    this->m_movementSpeed = movementSpeed;
    damage = 0;
    isDead = false;
    activeEffects = std::vector<Effect<Entity>*>();
    m_Effects = std::vector<Effect<Entity>*>();
}

void Entity::UpdateEffects(float dt)
{
    for (auto it = activeEffects.begin(); it != activeEffects.end();)
    {
        if (*it != nullptr) {
            (*it)->Update(dt);
            (*it)->Apply(*this);
            if ((*it)->IsExpired())
            {
                (*it)->Remove(*this);
                it = activeEffects.erase(it);
            }
            else
            {
              ++it;
            }
        }
    }
}