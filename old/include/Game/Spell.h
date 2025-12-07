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
#include "Events/BasicEvent.h"
#include "Game/Effect.h"
#include "Game/Entity.h"
#include "Game/ManaSystem.h"

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
enum class SpellEffect {
    multiple,
    pierce,
    freeze,
    chain,
    speed
};
struct SpellAssets {
    std::string baseSprite = "";
    std::string castSound = "";
    std::string hitSound = "";
    Animation* castAnimation;
    int frameCountW = -1;
    int frameCountH = -1;
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
	virtual float GetManaCost() = 0;
    virtual int GetElementCount() = 0;
    //virtual void ApplyEffects() = 0;
    virtual void IncreaseElementCount(int count) = 0;
    virtual void IncreaseManaCost(float factor) = 0;

    virtual void CastSpell(GameObject* caster) = 0;

    virtual void UpdateEffect(float dt) = 0;
    virtual void RemoveEffect(std::string effectName) = 0;

};

template <typename SpellStrategy>
class Spell
{
public:
    Spell(std::vector<std::string> modifiers, float baseManaCost, float baseDamage, float baseSpeed) : modifiers(modifiers), baseDamage(baseDamage), m_baseSpeed(baseSpeed), m_baseManaCost(baseManaCost) {}
    ~Spell() {};
    virtual SpellType GetSpellType() const = 0;
    virtual SpellElement GetSpellElement() const = 0;
    virtual const char *GetName() const = 0;
    virtual const char *GetElement() const = 0;
    virtual std::string ToString() const { return GetName(); }
    virtual float GetDamage() = 0;
    inline float GetManaCost() { return this->m_currentManaCost; };
    inline void IncreaseManaCost(float factor) {
        m_currentManaCost *= factor;
	}
    virtual void CastSpell(GameObject* caster) = 0;

	inline int GetElementCount() const{ return m_baseElementCount; }
    inline void IncreaseElementCount(int count)
    {
        m_baseElementCount += count;
	}
    virtual void AddEffect(std::shared_ptr<Effect<Spell<SpellStrategy>>> e) = 0;
    virtual void RemoveEffect(std::string effectName) = 0;
    virtual void UpdateEffect(float dt) = 0;
    virtual void AddOnHitEffect(Effect<Entity>* effect) {
        m_onHitEffects.push_back(effect);
	}
    inline void ApplyDamageFactor(float factor) {
		m_currentDamage = baseDamage * factor;
    }
    inline void ApplySpeedFactor(float factor) {
		m_currentSpeed = m_baseSpeed * factor;
	}
    inline void SetChainAmount(int amount) {
        m_chainAmount = amount;
    }
    inline int GetChainAmount() const {
        return m_chainAmount;
    }
    inline void SetPierceAmount(int amount) {
        m_pierceAmount = amount;
    }
    inline int GetPierceAmount() const {
        return m_pierceAmount;
    }

protected:
	std::vector<std::shared_ptr<Effect<Spell<SpellStrategy>>>> spellEffects;
	std::vector<Effect<Entity>*> m_onHitEffects;
    std::vector<std::string> modifiers;
    float m_baseManaCost;
    float m_currentManaCost = m_baseManaCost;
    float baseDamage;
	float m_currentDamage = baseDamage;
    std::string baseSprite;
    std::vector<SpellStrategy> m_elements;
	int m_baseElementCount = 1;
	float m_baseSpeed = 100.0f;
    int m_chainAmount = 0;
    int m_pierceAmount = 0;
    int m_currentSpeed = m_baseSpeed;

};

class Projectile : public Component, public Observer
{
public:
    Projectile(GameObject& associated, float speed, float distanceLeft, float damage) : Component(associated), m_currentSpeed(speed), m_distanceLeft(distanceLeft), m_baseDamage(damage) {
        this->associated.subject.addObserver(this);

        std::vector<std::string> layers;
        std::shared_ptr<Collider> collider = std::make_shared<Collider>(associated, std::vector<std::string>{"layer0"}, "projectile", new OnCollisionEvent(associated), Vec2{10,10});
        associated.AddComponent(collider);

    }
    std::shared_ptr<Sound> m_soundEffect;
    std::shared_ptr<Sound> m_hitSoundEffect;
    inline void RequestDelete() { m_requestDelete = true; }
    bool m_requestDelete = false;
    inline void SetChainAmount(int amount) {
        this->m_chainAmount = amount;
    }
    inline void SetPierceAmount(int amount) {
        this->m_pierceAmount = amount;
    }
    inline void SetSpeed(float speed) {
        m_currentSpeed = speed;
	}
    inline void SetOnHitEffects(std::vector< Effect<Entity>*> effects) {
        m_onHitEffects = effects;
    }
protected:
    //std::vector<std::weak_ptr<Effect>> spellEffects;
    inline void Start() {}
    inline void Update(float dt)
    {
        if (m_requestDelete) {
			std::shared_ptr<Collider> c = std::dynamic_pointer_cast<Collider>(this->associated.GetComponent("Collider").lock());
            c->disabled = true;
            std::shared_ptr<SpriteRenderer> sr = std::dynamic_pointer_cast<SpriteRenderer>(this->associated.GetComponent("SpriteRenderer").lock());
			sr->enabled = false;
            if (!this->m_hitSoundEffect->IsPlaying()) {
				this->associated.RequestDelete();
            }
            return;
        }
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

        GameObject* go = &evt.GetGameObject();
        if (std::find(lastHit.begin(), lastHit.end(), go) != lastHit.end()) return true;
		else lastHit.push_back(go);
        OnDamageTakenEvent e = OnDamageTakenEvent(this->associated, m_baseDamage);
        if (go->GetComponent("Character").lock()) {
            return true;
        }
        if (go->GetComponent("HealthSystem").lock()) {
            m_hitSoundEffect->Play();

            std::vector<Effect<Entity>*> clonedEffects;
            for (Effect<Entity>* effect : m_onHitEffects) {
                if (effect) {
                    clonedEffects.push_back(effect->Clone());
                }
            }
            OnEffectEvent<Entity>* effectEvt = new OnEffectEvent<Entity>(clonedEffects);
		    go->subject.notify(*effectEvt);
            go->subject.notify(e);
        }
        Vec2 currentPos = this->associated.box.center();
        std::weak_ptr<Entity> entity = Entity::GetClosestEnemy(currentPos, 300, lastHit);

        if (auto shared = entity.lock()) {
            if (m_chainAmount > 0) {
                m_chainAmount--;
                    float newAngle = Vec2::Angle(currentPos, shared->GetPosition());
				    this->associated.angleDeg = newAngle + 90;
            }
        }
        else if (!go->GetComponent("FireProjectileSpell").lock() && m_pierceAmount <=0)
            this->RequestDelete();
        m_pierceAmount--;
        return true;
    }

    inline void OnEvent(Event& evt)
    {
        EventDispatcher dispatcher(evt);

        dispatcher.Dispatch<OnCollisionEvent>(BIND_EVENT_FN(Projectile::OnCollision));
    };
    float m_currentSpeed;
    float m_distanceLeft;
    float m_baseDamage;
	std::vector<GameObject*> lastHit;

private:
    std::vector<Effect<Entity>*> m_onHitEffects;
    int m_pierceAmount;
    int m_chainAmount;
    std::weak_ptr<GameObject> m_particlesSystem;
    ParticleData m_Particle;
};

class ProjectileSpell : public Spell<Projectile>
{
public:
    ProjectileSpell(Vec2 initialPos, Vec2 target, SpellAssets assets) : Spell({}, 30, 500, 200), m_initialPos(initialPos), m_target(target), m_assets(assets) {
    }

    ~ProjectileSpell() {}
    SPELL_TYPE(projectile, fire);

    inline float GetDamage() { return m_currentDamage; };
    inline void CastSpell(GameObject *caster)
    {
        for (auto e : this->spellEffects) {
            //if(auto shared = e.lock())
				e->Apply(*this);
        }
        if (auto ms = std::dynamic_pointer_cast<ManaSystem>(caster->GetComponent("ManaSystem").lock())) {
            if (ms->GetMana() < this->GetManaCost()) {
                return;
            }
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
            std::shared_ptr<Projectile> spell = std::make_shared<Projectile>(*spellObj, m_currentSpeed, 300.0f, this->GetDamage());

            spell->SetChainAmount(this->GetChainAmount());
            spell->SetPierceAmount(this->GetPierceAmount());
            std::vector<Effect<Entity>*> clonedEffects;
            for (Effect<Entity>* effect : m_onHitEffects) {
                if (effect) {
                    clonedEffects.push_back(effect->Clone());
                }
            }
            spell->SetOnHitEffects(clonedEffects);



            spell->m_hitSoundEffect = std::make_shared<Sound>(m_assets.hitSound);

            std::shared_ptr<SpriteRenderer> sr = std::make_shared<SpriteRenderer>(*spellObj, m_assets.baseSprite, m_assets.frameCountW, m_assets.frameCountH, -90);
            spell->m_soundEffect = std::make_shared<Sound>(m_assets.castSound);
            spell->m_soundEffect->Play();

            spellObj->AddComponent(sr);
            spellObj->AddComponent(spell);
            spellObj->AddComponent(animator);
            Game::GetInstance().GetCurrentState()->AddObject(spellObj);
			animator->AddAnimation("projectile", m_assets.castAnimation);
			animator->SetAnimation("projectile");
            OnCastSpellEvent* newEvt = new OnCastSpellEvent(this->GetManaCost());
			caster->subject.notify(*newEvt);

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
                it++;
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
    SpellAssets m_assets;
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
        if (go.GetComponent("Character").lock()) {
            return true;
        }
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
	AreaSpell(Vec2 pos, SpellAssets assets) : Spell({}, 30, 10, 0), m_pos(pos), m_assets(assets)
    {}
	~AreaSpell() {}
    SPELL_TYPE(area, fire);
	inline float GetDamage() { return this->baseDamage; }
    inline void CastSpell(GameObject* caster) {
        if (auto ms = std::dynamic_pointer_cast<ManaSystem>(caster->GetComponent("ManaSystem").lock())) {
            if (ms->GetMana() < this->GetManaCost()) {
                return;
            }
        }
        std::shared_ptr<GameObject> spellObj = std::make_shared<GameObject>();
        std::shared_ptr<Animator> animator = std::make_shared<Animator>(*spellObj);
        spellObj->box.Move(m_pos);
        spellObj->z = 2;
        std::shared_ptr<SpriteRenderer> sr = std::make_shared<SpriteRenderer>(*spellObj, m_assets.baseSprite, m_assets.frameCountW, m_assets.frameCountH);
        std::shared_ptr<Area> spell = std::make_shared<Area>(*spellObj, 10, m_pos, spellObj->box.GetSize(), 1.0f);
        spell->m_soundEffect = std::make_shared<Sound>(m_assets.castSound);
        spell->m_soundEffect->Play();

        spellObj->AddComponent(sr);
        spellObj->AddComponent(spell);
        spellObj->AddComponent(animator);
        Game::GetInstance().GetCurrentState()->AddObject(spellObj);
        animator->AddAnimation("area", m_assets.castAnimation);
        animator->SetAnimation("area");
    }
    inline bool Is(std::string type) { return type == "AreaSpell"; }
    inline void AddEffect(std::shared_ptr<Effect<Spell<Area>>> e) override {
        if (e) {
            for (const auto& effect : spellEffects) {
                if (effect->GetName() == e->GetName()) {
                    return; // j� existe
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
    SpellAssets m_assets;
};