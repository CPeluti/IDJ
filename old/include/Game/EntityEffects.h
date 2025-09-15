#include "Effect.h"
#include "Entity.h"
#include "Core/Timer.h"
class FreezeEffect : public Effect<Entity> {
public:
	FreezeEffect(float duration) : m_duration(duration) {}
	Effect<Entity>* Clone() const override {
		return new FreezeEffect(*this);
	}
	void Apply(Entity& entity) {
		if (!applied) {
			entity.Freeze(true);
			this->m_duration.Restart();
		}
		applied = true;
	}
	void Update(float dt) {
		m_duration.Update(dt);

	}
	void Remove(Entity& entity) {
		entity.Freeze(false);
	}
	bool IsExpired() { return m_duration.Expired(); };
	EFFECT_TYPE(Entity, FreezeEffect);
	std::string ToString() const { return GetName(); }
	int getDuration() const {
		return m_duration.GetAmount();
	}
protected:
	Timer m_duration;
	bool applied = false;
};