#pragma once
#include "Core/Vec2.h"
#include "Core/Timer.h"
#include "Core/GameObject.h"
#include "Effect.h"
#include <vector>
#include <memory>


class Entity
{
public:
	enum class EnemyType {
		None,
		Enemy,
	};
	Entity();
	Entity(float speed);
	inline virtual void AddEffect(std::weak_ptr<Effect<Entity>> effect) { activeEffects.push_back(std::move(effect)); };
	virtual void UpdateEffects(float dt);
	inline virtual void SetMovementSpeed(const float &newSpeed) { m_movementSpeed = newSpeed; };
	inline virtual float GetMovementSpeed() const { return m_movementSpeed; };
	inline Vec2 GetPosition() const { return m_associated->box.center(); };
	inline void SetTargeted(bool targeted) { m_targeted = targeted; }
	inline static std::weak_ptr<Entity> GetClosestEnemy(Vec2 reference, float range) {
		float minDistance = std::numeric_limits<float>::max();
		std::weak_ptr<Entity> closestEnemy;
		for(auto &enemy : m_enemies) {
			if (auto e = enemy.lock()) {
				auto distance = Vec2::Distance(e->m_associated->box.center(), reference);
				if (e->type == EnemyType::Enemy && !e->isDead && distance <= minDistance && distance <= range) {
					minDistance = distance;
					closestEnemy = e;
				}
			}
		}
		return closestEnemy;
	}
protected:
	bool m_targeted;
	GameObject* m_associated;
	float m_movementSpeed;
	int damage;
	bool isDead;
	EnemyType type = EnemyType::None;
	Timer deathTimer;
	std::vector<std::shared_ptr<Effect<Entity>>> m_Effects;
	std::vector<std::weak_ptr<Effect<Entity>>> activeEffects;
	static std::vector<std::weak_ptr<Entity>> m_enemies;
};