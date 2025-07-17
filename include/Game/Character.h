#pragma once
#include "Core/GameObject.h"
#include "Core/Subject.h"
#include "Core/Timer.h"
#include "Core/TileMap.h"
#include "Spell.h"

#include "Entity.h"

#include <memory>
#include <queue>
struct Direction {
    Vec2 coordinates;
    std::string name;
};
class Character : public Component, public Observer, public Entity, public std::enable_shared_from_this<Character>
{
public:
    class Command
    {
    public:
        enum CommandType
        {
            MOVE,
            SHOOT,
            DASH
        };
        Command(CommandType type, Vec2 pos);
        CommandType type;
        Vec2 pos;
    };
    Character(GameObject &associated, std::string sprite, std::weak_ptr<TileMap> tilemap, bool isPlayer = false);
    ~Character();
    void Start();
    void Update(float dt);
    void Render();
    inline void Issue(Command task) { taskQueue.push(task); }

    // void Damage(int amount);

    // void OnNotify(Component& component);
    void OnEvent(Event &evt);

    inline Vec2 GetPos() const { return associated.box.center(); }
    inline int getHP() const { return hp; }
    inline int getProjectileNumber() const { return extraProjectiles; }

    inline void SetFlip(bool value) { flip = value; }

    inline bool Is(std::string type) { return type == "Character"; }

    void CastSpell(SpellType type, SpellElement element, std::vector<std::shared_ptr<IEffect>>, Vec2 target);

    void SetAnimation(Vec2 direction, std::string prefix);
    // COMPONENT_IS("Character");

public:
    static int npcCounter;
    static std::weak_ptr<Character> player;

private:
    bool OnCollision(OnCollisionEvent &evt);
    bool OnDamageTaken(OnDamageTakenEvent &evt);
    bool OnEffect(OnEffectEvent<Entity> &evt);

private:
    bool flip;
    Subject subject;
    std::weak_ptr<GameObject> gun;
    std::queue<Command> taskQueue;
    Vec2 speed;
    float hp;
    float mana;
    bool isDead;
    Timer deathTimer;
    int extraProjectiles;
	std::vector<std::shared_ptr<IEffect>> effects;
    Direction m_lastDirection;
    Timer m_dashTimer;
	Timer m_idleTimer;

	std::weak_ptr<Entity> targetedEnemy;

	std::weak_ptr<TileMap> tilemap;

    //std::map<OnHitEffects, std::shared_ptr<Effect<Entity>>> m_onHitEffects;
};


inline std::string format_as(const Direction& d)
{
    return fmt::format("{}, {}", d.coordinates, d.name);
}