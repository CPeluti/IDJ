#pragma once
#include <string>
#include <vector>
#include "Core/SpriteRenderer.h"
#include "Core/ParticleSystem.h"
#include "Core/Collider.h"
#include "Core/Camera.h"
#include "Core/Game.h"
#include "Core/Animator.h"
#include "Core/Sound.h"
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
    Projectile(GameObject& associated, float speed, float distanceLeft, float damage) : Component(associated), m_baseSpeed(speed), m_currentSpeed(speed), m_distanceLeft(distanceLeft), m_baseDamage(damage) {
        this->associated.subject.addObserver(this);

        std::vector<std::string> layers;
        std::shared_ptr<Collider> collider = std::make_shared<Collider>(associated, std::vector<std::string>{"layer0"}, "projectile", new OnCollisionEvent(associated), Vec2{10,10});
        associated.AddComponent(collider);

    }
    std::shared_ptr<Sound> m_soundEffect;
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
        if (go.GetComponent("Character").lock()) {
            return true;
        }
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
    ProjectileSpell(Vec2 initialPos, Vec2 target) : Spell({}, 10, "resources/img/fogo_projetil.png"), m_initialPos(initialPos), m_target(target) {
    }
    
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
			int halfCount = this->GetElementCount() / 2;
            float angleStep = 10 * (i - halfCount);
            //float angleStep = 10*i;
            float startingAngle = Vec2::Angle(m_initialPos, m_target);
            std::shared_ptr<GameObject> spellObj = std::make_shared<GameObject>();
            std::shared_ptr<Animator> animator = std::make_shared<Animator>(*spellObj);
		    spellObj->box.Move(m_initialPos);
		    spellObj->angleDeg = startingAngle+90+angleStep;
            std::shared_ptr<Projectile> spell = std::make_shared<Projectile>(*spellObj, 50.0f, 300.0f, this->GetDamage());
            std::shared_ptr<SpriteRenderer> sr = std::make_shared<SpriteRenderer>(*spellObj, baseSprite, 8, 2, -90);
            spell->m_soundEffect = std::make_shared<Sound>("resources/audio/FireBall_Cast.wav");
            spell->m_soundEffect->Play();

            spellObj->AddComponent(sr);
            spellObj->AddComponent(spell);
            spellObj->AddComponent(animator);
            Game::GetInstance().GetCurrentState()->AddObject(spellObj);
			animator->AddAnimation("projectile", new Animation(0, 7, 0.1f));
			animator->SetAnimation("projectile");
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

    inline bool Is(std::string type) { return type == "ProjectileSpell"; }

private:
    Vec2 m_initialPos;
	Vec2 m_target;
};


class Area : public Component, public Observer
{
public:
    Area(GameObject& associated, float damage, Vec2 pos, Vec2 size, float duration) : Component(associated), m_pos(pos), m_size(size), m_duration(duration), m_baseDamage(damage) {
        this->associated.subject.addObserver(this);

        std::vector<std::string> layers;
        std::shared_ptr<Collider> collider = std::make_shared<Collider>(associated, std::vector<std::string>{"layer0"}, "area", new OnCollisionEvent(associated), size, Vec2{1,1});
        associated.AddComponent(collider);

    }
    std::shared_ptr<Sound> m_soundEffect;
protected:
    //std::vector<std::weak_ptr<Effect>> spellEffects;
    inline void Start() {
        m_duration.Restart();
    }
    inline void Update(float dt)
    {

        associated.box.Move(m_pos);
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
    inline void Render()
    {
    }
    inline bool Is(std::string type) { return type == "Area"; }
    inline bool OnCollision(OnCollisionEvent& evt)
    {
        GameObject& go = evt.GetGameObject();
        OnDamageTakenEvent e = OnDamageTakenEvent(this->associated, this->m_baseDamage);
        if (go.GetComponent("HealthSystem").lock())
            go.subject.notify(e);

        return true;
    }

    inline void OnEvent(Event& evt)
    {
        EventDispatcher dispatcher(evt);

        dispatcher.Dispatch<OnCollisionEvent>(BIND_EVENT_FN(Area::OnCollision));
    };

    float m_baseDamage;
    Vec2 m_pos;
    Vec2 m_size;
    Timer m_duration;

private:
    std::weak_ptr<GameObject> m_particlesSystem;
    ParticleData m_Particle;
};


class AreaSpell : public Spell<Area> {
public:
    AreaSpell(Vec2 pos) : Spell({}, 10, "resources/img/fogo_area.png"), m_pos(pos)
    {}
	~AreaSpell() {}
    SPELL_TYPE(area, fire);
	inline float GetDamage() { return this->baseDamage; }
    inline void CastSpell() {
        std::shared_ptr<GameObject> spellObj = std::make_shared<GameObject>();
        std::shared_ptr<Animator> animator = std::make_shared<Animator>(*spellObj, false);
        spellObj->box.Move(m_pos);
        spellObj->z = 2;
        std::shared_ptr<SpriteRenderer> sr = std::make_shared<SpriteRenderer>(*spellObj, baseSprite, 10, 1);
        std::shared_ptr<Area> spell = std::make_shared<Area>(*spellObj, 10, m_pos, spellObj->box.GetSize(), 1.0f);
        spell->m_soundEffect = std::make_shared<Sound>("resources/audio/FireArea_Explode.wav");
        spell->m_soundEffect->Play();

        spellObj->AddComponent(sr);
        spellObj->AddComponent(spell);
        spellObj->AddComponent(animator);
        Game::GetInstance().GetCurrentState()->AddObject(spellObj);
        animator->AddAnimation("area", new Animation(0, 9, 0.1f));
        animator->SetAnimation("area");
    }
    inline bool Is(std::string type) { return type == "FireAreaSpell"; }
    inline void AddEffect(std::shared_ptr<Effect<Spell<Area>>> e) override {
        if (e) {
            for (const auto& effect : spellEffects) {
                if (effect->GetName() == e->GetName()) {
                    return; // já existe
                }
            }
            spellEffects.push_back(e);
        }
    }

    inline void RemoveEffect(std::string effectName) override {
        for (auto it = spellEffects.begin(); it != spellEffects.end(); ) {
            if ((*it)->GetName() == effectName) {
                it = spellEffects.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    inline void UpdateEffect(float dt) override {
        for (auto it = spellEffects.begin(); it != spellEffects.end(); ) {
            if (auto e = *it) {
                e->Update(dt);
                if (e->IsExpired()) {
                    it = spellEffects.erase(it);
                }
                else {
                    ++it;
                }
            }
            else {
                it = spellEffects.erase(it);
            }
        }
    }


    bool targetsPlayer = false;
private:
   
	std::weak_ptr<GameObject> m_particlesSystem;
    ParticleData m_Particle;
    Vec2 m_pos;
};