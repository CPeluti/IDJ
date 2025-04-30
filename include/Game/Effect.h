#pragma once

class Entity;

class Effect
{
public:
  virtual ~Effect() = default;
  virtual void Apply(Entity &entity, float dt) = 0;
  virtual bool IsExpired() = 0;
};