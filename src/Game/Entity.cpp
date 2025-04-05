#include "Core/Timer.h"

#include "Game/Entity.h"

Entity::Entity() : deathTimer(5)
{
  speed = Vec2(0, 0);
  damage = 0;
  isDead = false;
}