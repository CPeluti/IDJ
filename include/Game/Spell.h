// class Projectile{};
#include <string>
#include <vector>
#include "core/SpriteRenderer.h"
#include "Core/ParticleSystem.h"
#include "Core/Collider.h"

#define SPELL_TYPE(type, element) static SpellType GetStaticType() { return SpellType::type; }\
                            virtual SpellType GetSpellType() const override { return GetStaticType(); }\
                            virtual const char* GetName() const override { return #type; }\
                            static SpellElement GetStaticElement() { return SpellElement::element; }\
                            virtual SpellElement GetSpellElement() const override { return GetStaticElement(); }\
                            virtual const char* GetElement() const override { return #element; }\

enum class SpellType {
    projectile,
    area
};
enum class SpellElement{
    fire,
    water
};

class Spell {
    public:
        Spell(GameObject& associated, std::vector<std::string> modifiers, float baseDamage, std::string baseSprite):modifiers(modifiers), baseDamage(baseDamage), baseSprite(baseSprite) {}
        ~Spell(){};
        virtual SpellType GetSpellType() const = 0;
        virtual SpellElement GetSpellElement() const = 0;
        virtual const char* GetName() const = 0;
        virtual const char* GetElement() const = 0;
        virtual std::string ToString() const {return GetName();}
        virtual float GetDamage() = 0;
        virtual void CastSpell(){};

    protected:
        std::vector<std::string> modifiers;
        float baseDamage;
        std::string baseSprite;

};

class Projectile {
    public:
        Projectile(float speed, float distanceLeft):m_speed(speed), m_distanceLeft(distanceLeft){}
    protected:
        inline void SpellTypeStrategy(GameObject& associated, float dt){
            
            Vec2 rotatedSpeed = Vec2::Rotate({.0, -m_speed}, associated.angleDeg);

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



class FireProjectileSpell : public Spell, public Projectile, public Component, public Observer{
    public:
        FireProjectileSpell(GameObject &associated, Vec2 initialPos):
            Spell(associated, {}, 10, "resources/img/Bullet.png"),
            Projectile(350,100),
            Component(associated)
        {

            this->associated.subject.addObserver(this);
            
            SpriteRenderer *sr = new SpriteRenderer(associated, baseSprite, 1, 1);
            this->associated.box.Move(initialPos);
            this->associated.angleDeg = angle+90;
            
            this->associated.AddComponent(sr);

            std::vector<std::string> layers;
            layers.push_back("layer0");
            Collider *collider = new Collider(associated, layers, new OnCollisionEvent(associated));
            associated.AddComponent(collider);


        }
        ~FireProjectileSpell(){}
        SPELL_TYPE(projectile, fire);
        inline float GetDamage() {return this->baseDamage;}
        inline void Update(float dt){
            this->SpellTypeStrategy(this->associated, dt);
        };
        inline void Render(){};
        inline void Start(){};
        inline bool Is(std::string type) {return type == "FireProjectileSpell";}
        inline void OnEvent(Event& evt){
            EventDispatcher dispatcher(evt);

            dispatcher.Dispatch<OnCollisionEvent>(BIND_EVENT_FN(FireProjectileSpell::OnCollision));
        };
        bool targetsPlayer = false;
    private:
        inline bool OnCollision(OnCollisionEvent& evt){    
            GameObject &go = evt.GetGameObject();
            OnDamageTakenEvent e = OnDamageTakenEvent(this->associated, this->baseDamage);
            if (go.GetComponent("HealthSystem"))
                go.subject.notify(e);
            if (!go.GetComponent("Bullet"))
                this->associated.RequestDelete();

            return true;
        }

    private:
        std::weak_ptr<GameObject> m_particlesSystem;
        ParticleData m_Particle;
};