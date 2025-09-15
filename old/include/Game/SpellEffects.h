#pragma once
#include "Effect.h"
#include "EntityEffects.h"
#include "Spell.h"
#include "spdlog/fmt/fmt.h"

class MoreProjectileEffect : public Effect<Spell<Projectile>>{
public:
	MoreProjectileEffect(int amount): m_extraProjectiles(amount) {}
	Effect<Spell<Projectile>>* Clone() const override {
		return new MoreProjectileEffect(*this);
	}
	void Apply(Spell<Projectile> &spell) {
		if(!applied){
			spell.IncreaseElementCount(m_extraProjectiles);
		}
		applied = true;
	}
	void Update(float dt){}
	bool IsExpired() { return false; };
	EFFECT_TYPE(Projectile, MoreProjectileEffect);
	std::string ToString() const { return GetName(); }
	int getExtraProjectiles() const {
		return m_extraProjectiles;
	}
protected:
	int m_extraProjectiles;
	bool applied = false;
};

class ChainEffect : public Effect<Spell<Projectile>> {
public:
	ChainEffect(int amount) : m_chainAmount(amount) {}
	Effect<Spell<Projectile>>* Clone() const override {
		return new ChainEffect(*this);
	}
	void Apply(Spell<Projectile>& spell) {
		if (!applied) {
			spell.SetChainAmount(m_chainAmount);
 		}
		applied = true;
	}
	void Update(float dt) {}
	bool IsExpired() { return false; };
	EFFECT_TYPE(Projectile, ChainEffect);
	std::string ToString() const { return GetName(); }
	int GetChainAmount() const {
		return m_chainAmount;
	}
protected:
	int m_chainAmount;
	bool applied = false;
};

class PierceEffect : public Effect<Spell<Projectile>> {
public:
	PierceEffect(int amount) : m_pierceAmount(amount) {}
	Effect<Spell<Projectile>>* Clone() const override {
		return new PierceEffect(*this);
	}
	void Apply(Spell<Projectile>& spell) {
		if (!applied) {
			spell.SetPierceAmount(m_pierceAmount);
		}
		applied = true;
	}
	void Update(float dt) {}
	bool IsExpired() { return false; };
	EFFECT_TYPE(Projectile, ChainEffect);
	std::string ToString() const { return GetName(); }
	int GetChainAmount() const {
		return m_pierceAmount;
	}
protected:
	int m_pierceAmount;
	bool applied = false;
};

class SpeedEffect : public Effect<Spell<Projectile>> {
public:
	SpeedEffect(int amount) : m_speedFactor(amount) {}
	Effect<Spell<Projectile>>* Clone() const override {
		return new SpeedEffect(*this);
	}
	void Apply(Spell<Projectile>& spell) {
		if (!applied) {
			spell.ApplySpeedFactor(m_speedFactor);
		}
		applied = true;
	}
	void Update(float dt) {}
	bool IsExpired() { return false; };
	EFFECT_TYPE(Projectile, ChainEffect);
	std::string ToString() const { return GetName(); }
	int GetChainAmount() const {
		return m_speedFactor;
	}
protected:
	int m_speedFactor;
	bool applied = false;
};

class FreezeOnHitEffect : public Effect<Spell<Projectile>> {
public:
	FreezeOnHitEffect(Effect<Entity>* freezeEffect): m_freezeEffect(freezeEffect) {}
	Effect<Spell<Projectile>>* Clone() const override {
		return new FreezeOnHitEffect(*this);
	}
	void Apply(Spell<Projectile>& spell) {
		if (!applied) {
			spell.AddOnHitEffect(m_freezeEffect);
		}
		applied = true;
	}
	void Update(float dt) {}
	bool IsExpired() { return false; };
	EFFECT_TYPE(Projectile, FreezeEffect);
	std::string ToString() const { return GetName(); }
	Effect<Entity>* GetFreezeEffect() const {
		return m_freezeEffect;
	}
protected:
	Effect<Entity>* m_freezeEffect;
	bool applied = false;
};


inline std::string format_as(const MoreProjectileEffect& e)
{
	return fmt::format("Effect: {}\n\t extraProjectiles:{}",e.ToString(), e.getExtraProjectiles());
}