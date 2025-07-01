#pragma once
#include "Core/Vec2.h"
#include "Core/Timer.h"
#include "Effect.h"
#include <vector>
#include <memory>

class Entity
{
public:
  Entity();
  Entity(float speed);
  inline virtual void AddEffect(std::weak_ptr<Effect<Entity>> effect) { activeEffects.push_back(std::move(effect)); };
  virtual void UpdateEffects(float dt);
  inline virtual void SetMovementSpeed(const float &newSpeed) { m_movementSpeed = newSpeed; };
  inline virtual float GetMovementSpeed() const { return m_movementSpeed; };

protected:
  float m_movementSpeed;
  int damage;
  bool isDead;
  Timer deathTimer;
  std::vector<std::shared_ptr<Effect<Entity>>> m_Effects;
  std::vector<std::weak_ptr<Effect<Entity>>> activeEffects;
};