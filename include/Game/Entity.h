#pragma once
#include "Core/Vec2.h"
#include "Core/Timer.h"
#include "Core/GameObject.h"
#include "Effect.h"
#include "SDL_gpu.h"
#include <vector>
#include <memory>

inline void update_color_shader(float r, float g, float b, float a, int color_loc)
{
	float fcolor[4] = {r, g, b, a};
	GPU_SetUniformfv(color_loc, 4, 1, fcolor);
}


class Entity
{
public:
	enum class EnemyType
	{
		None,
		Enemy,
	};
	Entity();
	Entity(float speed);
	inline virtual void AddEffect(Effect<Entity>* effect) { 
		for(int i = 0; i < activeEffects.size(); i++)
		{
			//if (auto e = activeEffects[i].lock()){
			//	if (e == effect.lock()) return; // Effect already exists
			//}
		}
		activeEffects.push_back(std::move(effect));
	};
	virtual void UpdateEffects(float dt);
	virtual void Freeze(bool toggle) { m_freeze = toggle; };
	inline virtual void SetMovementSpeed(const float &newSpeed) { m_movementSpeed = newSpeed; };
	inline virtual float GetMovementSpeed() const { return m_movementSpeed; };
	inline Vec2 GetPosition() const { return m_associated->box.center(); };
	inline void SetTargeted(bool targeted) { m_targeted = targeted; }
	inline static std::weak_ptr<Entity> GetClosestEnemy(Vec2 reference, float range, std::vector<GameObject*> ignore = {})
	{
		float minDistance = std::numeric_limits<float>::max();
		std::weak_ptr<Entity> closestEnemy;
		for (auto &enemy : m_enemies)
		{
			if (auto e = enemy.lock())
			{
				if (!e->m_associated) continue;
				if(std::find(ignore.begin(), ignore.end(), e->m_associated) != ignore.end()) continue;
				auto distance = Vec2::Distance(e->m_associated->box.center(), reference);
				if (e->type == EnemyType::Enemy && !e->isDead && distance <= minDistance && distance <= range)
				{
					minDistance = distance;
					closestEnemy = e;
				}
			}
		}
		return closestEnemy;
	}

protected:
	bool m_targeted = false;
	GameObject *m_associated;
	float m_movementSpeed;
	int damage;
	bool isDead;
	EnemyType type = EnemyType::None;
	Timer deathTimer;
	std::vector<Effect<Entity>*> m_Effects;
	std::vector<Effect<Entity>*> activeEffects;
	static std::vector<std::weak_ptr<Entity>> m_enemies;
	bool m_freeze = false;
};