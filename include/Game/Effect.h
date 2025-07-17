#pragma once
// #include "Spell.h"
#include <string>

#define EFFECT_TYPE(type, name)                                              \
  static EffectType GetStaticType() { return EffectType::type; }                \
  virtual EffectType GetEffectType() const override { return GetStaticType(); } \
  virtual const char *GetName() const override { return #name; }

enum class EffectType{
  None,
  Entity,
  OnHit,
  Projectile,
};

class IEffect
{
public:
	virtual ~IEffect() = default;
	virtual bool IsExpired() = 0;
	virtual void Update(float dt) = 0;
	virtual EffectType GetEffectType() const = 0;
	virtual const char* GetName() const = 0;
	virtual std::string ToString() const { return GetName(); }
};

template<typename T>
class Effect : public IEffect
{
public:
	virtual ~Effect() = default;
	virtual void Apply(T& t) = 0;
	virtual void Remove(T& t) {};
	virtual bool IsExpired() = 0;
	virtual void Update(float dt) = 0;
	virtual EffectType GetEffectType() const = 0;
	virtual const char *GetName() const = 0;
	virtual std::string ToString() const { return GetName(); }
	virtual Effect<T>* Clone() const = 0;
};