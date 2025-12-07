class_name Spell

enum SpellType
{
    projectile,
    area
};
enum SpellElement
{
    fire,
    water
};
enum SpellEffect {
    multiple,
    pierce,
    freeze,
    chain,
    speed
};

@export var modifiers : Array[String]
@export var mana_cost : float
@export var damage : float
@export var speed : float





