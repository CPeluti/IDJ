// class Projectile{};
#pragma once
#include <string>
#include <vector>
#include "Core/SpriteRenderer.h"
#include "Core/ParticleSystem.h"
#include "Core/Collider.h"
#include "Core/Camera.h"
#include "Core/Game.h"
#include "Game/Effect.h"

#define SPELL_TYPE(type, element)                                                        \
    static SpellType GetStaticType() { return SpellType::type; }                         \
    virtual SpellType GetSpellType() const override { return GetStaticType(); }          \
    virtual const char *GetName() const override { return #type; }                       \
    static SpellElement GetStaticElement() { return SpellElement::element; }             \
    virtual SpellElement GetSpellElement() const override { return GetStaticElement(); } \
    virtual const char *GetElement() const override { return #element; }

enum class SpellType
{
    projectile,
    area
};
enum class SpellElement
{
    fire,
    water
};

class Spell
{
public:
    Spell(GameObject &associated, std::vector<std::string> modifiers, float baseDamage, std::string baseSprite) : modifiers(modifiers), baseDamage(baseDamage), baseSprite(baseSprite) {}
    ~Spell() {};
    virtual SpellType GetSpellType() const = 0;
    virtual SpellElement GetSpellElement() const = 0;
    virtual const char *GetName() const = 0;
    virtual const char *GetElement() const = 0;
    virtual std::string ToString() const { return GetName(); }
    virtual float GetDamage() = 0;
    virtual void CastSpell() {};

protected:
    std::vector<std::string> modifiers;
    float baseDamage;
    std::string baseSprite;
};

class Projectile
{
public:
    Projectile(float speed, float distanceLeft) : m_speed(speed), m_distanceLeft(distanceLeft) {}
    void AddEffect(std::weak_ptr<ProjectileEffect> pe)
    {
        bool exists = false;
        
        for (auto it = spellEffects.begin(); it != spellEffects.end();)
        {
            std::shared_ptr<ProjectileEffect> e = (*it).lock();
            std::shared_ptr<ProjectileEffect> peLocked = (*it).lock();
            
            if (e->GetName() == peLocked->GetName())
            {
                exists = true;
                break;
            }
        }
        if (!exists)
        {
            spellEffects.push_back(pe);
        }
    }
    void RemoveEffect(std::string effectName)
    {
        for (auto it = spellEffects.begin(); it != spellEffects.end();)
        {
            std::shared_ptr<ProjectileEffect> e = (*it).lock();
            if (e->GetName() == effectName)
                auto erased = spellEffects.erase(it);
            else
                ++it;
        }
    }

protected:
    std::vector<std::weak_ptr<ProjectileEffect>> spellEffects;
    inline void SpellTypeStrategy(GameObject &associated, float dt)
    {

        Vec2 rotatedSpeed = Vec2::Rotate({.0f, -m_speed}, associated.angleDeg);

        Vec2 oldPos = associated.box.GetPos();
        Vec2 newPos = {
            oldPos.x + (rotatedSpeed.x * dt),
            oldPos.y + (rotatedSpeed.y * dt)};

        associated.box.RawMove(newPos);

        m_distanceLeft -= Vec2::Distance(newPos, oldPos);

        if (m_distanceLeft <= 0)
        {
            associated.RequestDelete();
        }
    }
    float m_speed;
    float m_distanceLeft;
    float angle;
};

class FireProjectileSpell : public Spell, public Projectile, public Component, public Observer
{
public:
    FireProjectileSpell(GameObject &associated, Vec2 initialPos) : Spell(associated, {}, 10, "resources/img/Bullet.png"),
                                                                   Projectile(350, 300),
                                                                   Component(associated)
    {

        this->associated.subject.addObserver(this);

        std::shared_ptr<SpriteRenderer> sr = std::make_shared<SpriteRenderer>(associated, baseSprite, 1, 1);
        this->associated.box.Move(initialPos);
        this->associated.angleDeg = angle + 90;

        this->associated.AddComponent(sr);

        std::vector<std::string> layers;
        layers.push_back("layer0");
        std::shared_ptr<Collider> collider = std::make_shared<Collider>(associated, layers, new OnCollisionEvent(associated), Vec2{10,10});
        associated.AddComponent(collider);
    }
    ~FireProjectileSpell() {}
    SPELL_TYPE(projectile, fire);
    inline float GetDamage() { return this->baseDamage; }
    inline void Update(float dt)
    {
        this->SpellTypeStrategy(this->associated, dt);
    };
    inline void Render() {};
    inline void Start() {};
    inline bool Is(std::string type) { return type == "FireProjectileSpell"; }
    inline void OnEvent(Event &evt)
    {
        EventDispatcher dispatcher(evt);

        dispatcher.Dispatch<OnCollisionEvent>(BIND_EVENT_FN(FireProjectileSpell::OnCollision));
    };
    bool targetsPlayer = false;

private:
    inline bool OnCollision(OnCollisionEvent &evt)
    {
        GameObject &go = evt.GetGameObject();
        OnDamageTakenEvent e = OnDamageTakenEvent(this->associated, this->baseDamage);
        if (go.GetComponent("HealthSystem").lock())
            go.subject.notify(e);
        if (!go.GetComponent("Bullet").lock())
            this->associated.RequestDelete();

        return true;
    }

private:
    std::weak_ptr<GameObject> m_particlesSystem;
    ParticleData m_Particle;
};

class AreaSpell
{
public:
    AreaSpell(Vec2 area, Vec2 size, float duration) : m_area(area), m_size(size), m_duration(duration) {
		m_duration.Restart();
    }
    void AddEffect(std::weak_ptr<AreaEffect> ae)
    {
        bool exists = false;

        for (auto it = spellEffects.begin(); it != spellEffects.end();)
        {
            std::shared_ptr<AreaEffect> e = (*it).lock();
            std::shared_ptr<AreaEffect> aeLocked = (*it).lock();

            if (e->GetName() == aeLocked->GetName())
            {
                exists = true;
                break;
            }
        }
        if (!exists)
        {
            spellEffects.push_back(ae);
        }
    }
    void RemoveEffect(std::string effectName)
    {
        for (auto it = spellEffects.begin(); it != spellEffects.end();)
        {
            std::shared_ptr<AreaEffect> e = (*it).lock();
            if (e->GetName() == effectName)
                auto erased = spellEffects.erase(it);
            else
                ++it;
        }
    }

protected:
    std::vector<std::weak_ptr<AreaEffect>> spellEffects;
    inline void SpellTypeStrategy(GameObject& associated, float dt)
    {

        associated.box.SetPos(m_area);
        associated.box.SetSize(m_size);

        if (m_duration.Expired())
        {
            associated.RequestDelete();
        }
        else
        {
            m_duration.Update(dt);
		}
    }
    Vec2 m_area;
    Vec2 m_size;
    Timer m_duration;
};

class FireAreaSpell : public Spell, public AreaSpell, public Component, public Observer {
public:
    FireAreaSpell(GameObject& associated, Vec2 pos) : Spell(associated, {}, 10, "resources/img/fire_placeholder.png"),
        AreaSpell(pos, { 100, 100 }, 5.0f),
        Component(associated)
    {
		this->associated.subject.addObserver(this);

		std::shared_ptr<SpriteRenderer> sr = std::make_shared<SpriteRenderer>(associated, baseSprite, 1, 1);
        
		this->associated.AddComponent(sr);

        std::vector<std::string> layers;
        layers.push_back("layer0");
        std::shared_ptr<Collider> collider = std::make_shared<Collider>(associated, layers, new OnCollisionEvent(associated), Vec2{ 10,10 });
        associated.AddComponent(collider);

    }
	~FireAreaSpell() {}
    SPELL_TYPE(area, fire);
	inline float GetDamage() { return this->baseDamage; }
    inline void Update(float dt)
    {
        this->SpellTypeStrategy(this->associated, dt);
    };
    inline void Render() {};
    inline void Start() {};
    inline bool Is(std::string type) { return type == "FireAreaSpell"; }
    inline void OnEvent(Event& evt)
    {
        EventDispatcher dispatcher(evt);

        dispatcher.Dispatch<OnCollisionEvent>(BIND_EVENT_FN(FireAreaSpell::OnCollision));
    };
    bool targetsPlayer = false;
private:
    inline bool OnCollision(OnCollisionEvent& evt)
    {
        GameObject& go = evt.GetGameObject();
        OnDamageTakenEvent e = OnDamageTakenEvent(this->associated, this->baseDamage);
        if (go.GetComponent("HealthSystem").lock())
            go.subject.notify(e);

        return true;
    }
	std::weak_ptr<GameObject> m_particlesSystem;
    ParticleData m_Particle;
};