#pragma once
#include "Core/Vec2.h"
#include "Core/Timer.h"

class Entity
{
public:
  Entity();

private:
  Vec2 speed;
  int damage;
  bool isDead;
  Timer deathTimer;
};