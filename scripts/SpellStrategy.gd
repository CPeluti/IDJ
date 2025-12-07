@abstract
class_name SpellStrategy

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

@abstract
func get_spell_type() -> SpellType
@abstract
func get_spell_element() -> SpellElement
@abstract
func get_damage() -> float
@abstract
func get_mana_cost() -> float
@abstract
func GetElementCount() -> int
@abstract
func ApplyEffects() -> void
@abstract
func set_mana_cost(factor: float) -> void

@abstract
func cast_spell(target_pos: Vector3) -> void

@abstract
func update_effect(dt: float)
@abstract
func remove_effect(effect_name: String)
