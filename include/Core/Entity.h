#pragma once
#include "Vec2.h"
#include "Timer.h"

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