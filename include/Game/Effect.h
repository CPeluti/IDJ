#pragma once
// #include "Spell.h"

#define EFFECT_TYPE(type)                                              \
  static EffectType GetStaticType() { return EffectType::type; }                \
  virtual EffectType GetEffectType() const override { return GetStaticType(); } \
  virtual const char *GetName() const override { return #type; }

class Entity;
class Projectile;

enum EffectType{
  None,
  Slow
};

class Effect
{
public:
  virtual ~Effect() = default;
  virtual void Apply(Entity &entity, float dt) = 0;
  virtual bool IsExpired() = 0;
  virtual EffectType GetEffectType() const = 0;
  virtual const char *GetName() const = 0;
  virtual std::string ToString() const { return GetName(); }
};

class ProjectileEffect
{
public:
  virtual ~ProjectileEffect() = default;
  inline virtual void Apply(Projectile &projectile, float dt) {}
  inline virtual bool IsExpired() = 0;
  virtual EffectType GetEffectType() const = 0;
  virtual const char *GetName() const = 0;
  virtual std::string ToString() const { return GetName(); }
};