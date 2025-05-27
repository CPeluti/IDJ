// class Projectile{};
#include <string>
#include <vector>
#include "core/SpriteRenderer.h"
#include "Core/ParticleSystem.h"

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
        Spell(GameObject& associated, std::vector<std::string> modifiers, float baseDamage, SpriteRenderer baseSprite): associated(associated), modifiers(modifiers), baseDamage(baseDamage), baseSprite(baseSprite) {}
        virtual SpellType GetSpellType() const = 0;
        virtual SpellElement GetSpellElement() const = 0;
        virtual const char* GetName() const = 0;
        virtual const char* GetElement() const = 0;
        virtual std::string ToString() const {return GetName();}
        virtual float getDamage() = 0;
        static void castSpell(Vec2 pos);

    protected:
        GameObject& associated;
        std::vector<std::string> modifiers;
        float baseDamage;
        SpriteRenderer baseSprite;
};

class FireProjectileSpell : public Spell, public Component, public Observer{
    public:
        FireProjectileSpell(GameObject &associated):Spell(associated, {}, 10, {associated, "resources/img/Bullet.png", 1, 1}),Component(associated){}
        SPELL_TYPE(projectile, fire);
        inline float getDamage() {return this->baseDamage;}
        void Update(float dt);
        void Render();
        void Start();
        inline bool Is(std::string type) {return type == "FireProjectileSpell";}
        void OnEvent(Event& evt);
        bool targetsPlayer = false;
    private:
        bool OnCollision(OnCollisionEvent& evt);

    private:
        Vec2 speed;
        float distanceLeft;
        int damage;
        std::weak_ptr<GameObject> particlesSystem;
        ParticleData m_Particle;
};