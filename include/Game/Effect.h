#pragma once
#include "Spell.h"

class Entity;

class Effect
{
public:
  virtual ~Effect() = default;
  virtual void Apply(Entity &entity, float dt) = 0;
  virtual bool IsExpired() = 0;
};

class SpellEffect
{
  virtual ~SpellEffect() = default;
  virtual void Apply(Spell &spell, float dt) = 0;
  virtual bool IsExpired() = 0;
};