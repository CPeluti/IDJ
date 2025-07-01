#pragma once
#include <string>
#include <vector>
#include "Core/SpriteRenderer.h"
#include "Core/ParticleSystem.h"
#include "Core/Collider.h"
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

template <typename SpellStrategy>
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

    void AddEffect(std::weak_ptr<Effect<SpellType>> e)
    {
        bool exists = false;
        if (auto targetEffect = e.lock()) {
            for (auto it = spellEffects.begin(); it != spellEffects.end();)
            {
                std::shared_ptr<Effect<SpellStrategy>> effect = (*it).lock();

                if (targetEffect->GetName() == effect->GetName())
                {
                    exists = true;
                    break;
                }
            }
            if (!exists)
            {
                spellEffects.push_back(e);
            }
        }
    }
    void RemoveEffect(std::string effectName)
    {
        for (auto it = spellEffects.begin(); it != spellEffects.end();)
        {
            std::shared_ptr<Effect<SpellStrategy>> e = (*it).lock();
            if (e->GetName() == effectName)
                auto erased = spellEffects.erase(it);
            else
                ++it;
        }
    }

    void UpdateEffect(float dt) {
        for(auto it = spellEffects.begin(); it != spellEffects.end();)
        {
            std::shared_ptr<Effect<SpellStrategy>> e = (*it).lock();
            if (e)
            {
                e->Update(dt);
                if (e->IsExpired())
                    it = spellEffects.erase(it);
                else
                    ++it;
            }
            else
            {
                it = spellEffects.erase(it);
            }
		}
    }

protected:
	std::vector<std::weak_ptr<Effect<SpellStrategy>>> spellEffects;
    std::vector<std::string> modifiers;
    float baseDamage;
    std::string baseSprite;
};

class Projectile
{
public:
    Projectile(float speed, float distanceLeft) : m_baseSpeed(speed), m_actualSpeed(speed), m_distanceLeft(distanceLeft) {}

protected:
    //std::vector<std::weak_ptr<Effect>> spellEffects;
    inline void SpellTypeStrategy(GameObject &associated, float dt)
    {

        Vec2 rotatedSpeed = Vec2::Rotate({.0f, -m_actualSpeed}, associated.angleDeg);

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
    float m_projectileAmount;
    float m_actualSpeed;
    float m_baseSpeed;
    float m_distanceLeft;
};

class FireProjectileSpell : public Spell<Projectile>, public Projectile, public Component, public Observer
{
public:
    FireProjectileSpell(GameObject &associated, Vec2 initialPos) : Spell(associated, {}, 10, "resources/img/fire_placeholder.png"),
                                                                   Projectile(350, 300),
                                                                   Component(associated)
    {

        this->associated.subject.addObserver(this);

        std::shared_ptr<SpriteRenderer> sr = std::make_shared<SpriteRenderer>(associated, baseSprite, 1, 1);
        this->associated.box.Move(initialPos);

        this->associated.AddComponent(sr);

        std::vector<std::string> layers;
        layers.push_back("layer0");
        std::shared_ptr<Collider> collider = std::make_shared<Collider>(associated, layers, "spell", new OnCollisionEvent(associated), Vec2{10,10});
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
        if (!go.GetComponent("FireProjectileSpell").lock())
            this->associated.RequestDelete();

        return true;
    }

private:
    std::weak_ptr<GameObject> m_particlesSystem;
    ParticleData m_Particle;
};