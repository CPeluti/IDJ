#pragma once
// #include "Spell.h"
#include <string>

#define EFFECT_TYPE(type)                                              \
  static EffectType GetStaticType() { return EffectType::type; }                \
  virtual EffectType GetEffectType() const override { return GetStaticType(); } \
  virtual const char *GetName() const override { return #type; }

enum class EffectType{
  None,
  Slow,
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
	virtual void Apply(T &t) = 0;
	virtual bool IsExpired() = 0;
	virtual void Update(float dt) = 0;
	virtual EffectType GetEffectType() const = 0;
	virtual const char *GetName() const = 0;
	virtual std::string ToString() const { return GetName(); }
};