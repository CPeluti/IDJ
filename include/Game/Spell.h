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

class ISpell {
public:
    virtual ~ISpell() = default;

    virtual SpellType GetSpellType() const = 0;
    virtual SpellElement GetSpellElement() const = 0;
    virtual const char* GetName() const = 0;
    virtual const char* GetElement() const = 0;
    virtual std::string ToString() const = 0;
    virtual float GetDamage() = 0;
    virtual int GetElementCount() = 0;
    //virtual void ApplyEffects() = 0;
	virtual void IncreaseElementCount(int count) = 0;

    virtual void CastSpell() = 0;

    virtual void UpdateEffect(float dt) = 0;
    virtual void RemoveEffect(std::string effectName) = 0;
   
};

template <typename SpellStrategy>
class Spell
{
public:
    Spell(std::vector<std::string> modifiers, float baseDamage, std::string baseSprite) : modifiers(modifiers), baseDamage(baseDamage), baseSprite(baseSprite) {}
    ~Spell() {};
    virtual SpellType GetSpellType() const = 0;
    virtual SpellElement GetSpellElement() const = 0;
    virtual const char *GetName() const = 0;
    virtual const char *GetElement() const = 0;
    virtual std::string ToString() const { return GetName(); }
    virtual float GetDamage() = 0;
    virtual void CastSpell() = 0;

	inline int GetElementCount() const{ return m_baseElementCount; }
    inline void IncreaseElementCount(int count)
    {
        m_baseElementCount += count;
	}
    virtual void AddEffect(std::shared_ptr<Effect<Spell<SpellStrategy>>> e) = 0;
    virtual void RemoveEffect(std::string effectName) = 0;
    virtual void UpdateEffect(float dt) = 0;

protected:
	std::vector<std::shared_ptr<Effect<Spell<SpellStrategy>>>> spellEffects;
    std::vector<std::string> modifiers;
    float baseDamage;
    std::string baseSprite;
    std::vector<SpellStrategy> m_elements;
	int m_baseElementCount = 1;
};

class Projectile : public Component, public Observer
{
public:
    Projectile(GameObject& associated, std::string sprite, float speed, float distanceLeft, float damage) : Component(associated), m_baseSpeed(speed), m_currentSpeed(speed), m_distanceLeft(distanceLeft), m_baseDamage(damage) {
        this->associated.subject.addObserver(this);
        
        std::shared_ptr<SpriteRenderer> sr = std::make_shared<SpriteRenderer>(associated, sprite, 1, 1);
        this->associated.AddComponent(sr);

        std::vector<std::string> layers;
        std::shared_ptr<Collider> collider = std::make_shared<Collider>(associated, std::vector<std::string>{"layer0"}, "projectile", new OnCollisionEvent(associated), Vec2{10,10});
        associated.AddComponent(collider);

    }
protected:
    //std::vector<std::weak_ptr<Effect>> spellEffects;
    inline void Start() {}
    inline void Update(float dt)
    {

        Vec2 rotatedSpeed = Vec2::Rotate({.0f, -m_currentSpeed}, associated.angleDeg);

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
    inline void Render()
    {}
	inline bool Is(std::string type) { return type == "Projectile"; }
    inline bool OnCollision(OnCollisionEvent& evt)
    {
        GameObject& go = evt.GetGameObject();
        OnDamageTakenEvent e = OnDamageTakenEvent(this->associated, m_baseDamage);
        if (go.GetComponent("HealthSystem").lock())
            go.subject.notify(e);
        if (!go.GetComponent("FireProjectileSpell").lock())
            this->associated.RequestDelete();

        return true;
    }

    inline void OnEvent(Event& evt)
    {
        EventDispatcher dispatcher(evt);

        dispatcher.Dispatch<OnCollisionEvent>(BIND_EVENT_FN(Projectile::OnCollision));
    };

    float m_currentSpeed;
    float m_baseSpeed;
    float m_distanceLeft;
    float m_baseDamage;
private:
    std::weak_ptr<GameObject> m_particlesSystem;
    ParticleData m_Particle;
};

class ProjectileSpell : public Spell<Projectile>
{
public:
    ProjectileSpell(Vec2 initialPos, Vec2 target) : Spell({}, 10, "resources/img/fire_placeholder.png"), m_initialPos(initialPos), m_target(target){}
    
    ~ProjectileSpell() {}
    SPELL_TYPE(projectile, fire);


    inline float GetDamage() { return 30.0f; };
    inline void CastSpell()
    {
        for (auto e : this->spellEffects) {
            //if(auto shared = e.lock())
				e->Apply(*this);
        }
        for(int i = 0; i < this->GetElementCount(); i++)
        {
            float angleStep = 10*i;
            float startingAngle = Vec2::Angle(m_initialPos, m_target);
            std::shared_ptr<GameObject> spellObj = std::make_shared<GameObject>();
		    spellObj->box.Move(m_initialPos);
		    spellObj->angleDeg = startingAngle+90+angleStep;
            std::shared_ptr<Projectile> spell = std::make_shared<Projectile>(*spellObj, baseSprite, 50.0f, 300.0f, this->GetDamage());
            spellObj->AddComponent(spell);
            Game::GetInstance().GetCurrentState()->AddObject(spellObj);
		}
	}
    inline void AddEffect(std::shared_ptr<Effect<Spell<Projectile>>> e)
    {
        bool exists = false;
        if (e) {
            for (auto it = spellEffects.begin(); it != spellEffects.end();)
            {
                std::shared_ptr<Effect<Spell<Projectile>>> effect = (*it);

                if (e->GetName() == effect->GetName())
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
    inline void RemoveEffect(std::string effectName)
    {
        for (auto it = spellEffects.begin(); it != spellEffects.end();)
        {
            std::shared_ptr<Effect<Spell<Projectile>>> e = (*it);
            if (e->GetName() == effectName)
                auto erased = spellEffects.erase(it);
            else
                ++it;
        }
    }
    inline void UpdateEffect(float dt) {
        for (auto it = spellEffects.begin(); it != spellEffects.end();)
        {
            std::shared_ptr<Effect<Spell<Projectile>>> e = (*it);
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

    inline void Render() {};
    inline void Start() {};
    inline bool Is(std::string type) { return type == "ProjectileSpell"; }

private:
    Vec2 m_initialPos;
	Vec2 m_target;

};

class AreaSpell
{
public:
    AreaSpell(Vec2 area, Vec2 size, float duration) : m_area(area), m_size(size), m_duration(duration) {
		m_duration.Restart();
    }
protected:
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

//class FireAreaSpell : public Spell<AreaSpell>, public AreaSpell, public Observer {
//public:
//    FireAreaSpell(GameObject& associated, Vec2 pos) : Spell(associated, {}, 10, "resources/img/fire_placeholder.png"),
//        AreaSpell(pos, { 100, 100 }, 5.0f)
//    {
//
//
//    }
//	~FireAreaSpell() {}
//    SPELL_TYPE(area, fire);
//	inline float GetDamage() { return this->baseDamage; }
//    inline void Update(float dt)
//    {
//        //this->SpellTypeStrategy(this->associated, dt);
//    };
//    inline void Render() {};
//    inline void Start() {};
//    inline bool Is(std::string type) { return type == "FireAreaSpell"; }
//    inline void OnEvent(Event& evt)
//    {
//        EventDispatcher dispatcher(evt);
//
//        dispatcher.Dispatch<OnCollisionEvent>(BIND_EVENT_FN(FireAreaSpell::OnCollision));
//    };
//    bool targetsPlayer = false;
//private:
//    inline bool OnCollision(OnCollisionEvent& evt)
//    {
//        GameObject& go = evt.GetGameObject();
//        OnDamageTakenEvent e = OnDamageTakenEvent(this->associated, this->baseDamage);
//        if (go.GetComponent("HealthSystem").lock())
//            go.subject.notify(e);
//
//        return true;
//    }
//	std::weak_ptr<GameObject> m_particlesSystem;
//    ParticleData m_Particle;
//};