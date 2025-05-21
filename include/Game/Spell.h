// class Projectile{};
#include <string>
#include <vector>
#include "core/SpriteRenderer.h"

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
        virtual SpellType GetSpellType() const = 0;
        virtual SpellElement GetSpellElement() const = 0;
        virtual const char* GetName() const = 0;
        virtual const char* GetElement() const = 0;
        virtual std::string ToString() const {return GetName();}
        void castSpell();

    protected:
        std::vector<std::string> modifier;
};

class FireProjectileSpell : public Spell{
    public:
        FireProjectileSpell(){
            baseSprite
        }

        SPELL_TYPE(projectile, fire);
    protected:
        SpriteRenderer baseSprite;
};