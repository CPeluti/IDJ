#pragma once
#include "Effect.h"
#include "Spell.h"
#include "spdlog/fmt/fmt.h"

class MoreProjectileEffect : public Effect<Projectile>{
public:
	MoreProjectileEffect(int amount): m_extraProjectiles(amount) {}
	void Apply(Projectile &spell) {
		if(!applied){
			spell.addProjectiles(m_extraProjectiles);
		}
		applied = true;
	}
	void Update(float dt){}
	bool IsExpired() { return false; };
	EFFECT_TYPE(Projectile);
	std::string ToString() const { return GetName(); }
	int getExtraProjectiles() const {
		return m_extraProjectiles;
	}
protected:
	int m_extraProjectiles;
	bool applied = false;
};

inline std::string format_as(const MoreProjectileEffect& e)
{
	return fmt::format("Effect: {}\n\t extraProjectiles:{}",e.ToString(), e.getExtraProjectiles());
}