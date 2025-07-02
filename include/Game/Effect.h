#pragma once
// #include "Spell.h"

#define EFFECT_TYPE(type)                                              \
  static EffectType GetStaticType() { return EffectType::type; }                \
  virtual EffectType GetEffectType() const override { return GetStaticType(); } \
  virtual const char *GetName() const override { return #type; }

enum EffectType{
  None,
  Slow
};

template<typename T>
class Effect
{
public:
  virtual ~Effect() = default;
  virtual void Apply(T &t, float dt) = 0;
  virtual bool IsExpired() = 0;
  virtual EffectType GetEffectType() const = 0;
  virtual const char *GetName() const = 0;
  virtual std::string ToString() const { return GetName(); }
};