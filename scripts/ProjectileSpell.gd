class_name ProjectileSpell
extends SpellStrategy

var mana_cost : float
var speed : float
var damage : float

var instances : Array[Node3D]

var starting_position: Vector3

var modifiers : Array[String]
var projectile: PackedScene

var spell_element : SpellElement = SpellElement.fire
var base_damage = 2.0
var base_speed = 30.0
var speed_increase = 0.0
var projectile_amount : int = 1
var chain_amount: int = 0
var is_freezing_projectile: bool = false
var base_mana_cost : float = 10.0
var caster: CharacterBody3D

var gm: GameMaster

func _init(owner: CharacterBody3D, projectile: PackedScene, gm: GameMaster) -> void:
	self.caster = owner
	self.projectile = projectile
	self.gm = gm

func get_spell_type():
	return SpellType.projectile
func get_spell_element() -> SpellElement:
	return spell_element
func get_damage() -> float:
	return damage
func get_mana_cost() -> float:
	return mana_cost
func GetElementCount() -> int:
	return 1
func set_projectile_amount(count: int) -> void:
	projectile_amount = count
func set_mana_cost(factor: float) -> void:
	mana_cost = factor
func increase_projectile_amount(amount: int) -> void:
	projectile_amount += amount
func increase_projectile_speed(amount: float) -> void:
	speed_increase += amount
func set_projectile_as_freezing() -> void:
	is_freezing_projectile = true
func set_chain_amount(amount: int) -> void:
	chain_amount = amount
func update_effect(_dt: float):
	pass
func remove_effect(_effect_name: String):
	pass
func apply_effects() -> void:
	for e in effects:
		if (e.type == Effect.EffectType.Projectile):
			e.Apply(self)
func cast_spell(target_pos: Vector3) -> void:
	apply_effects()
	for i in projectile_amount:
		var instanced_entity: Projectile = projectile.instantiate()
		caster.get_parent().add_child(instanced_entity)
		instanced_entity.global_position = caster.global_position
		
		instanced_entity.caster = self.caster
		instanced_entity.gm = gm
		instanced_entity.chain_amount = chain_amount
		instanced_entity.base_speed = base_speed + speed_increase
		instanced_entity.is_freezing_projectile = is_freezing_projectile
		instanced_entity.base_attack_damage = base_damage
		var spread_angle = 30.0
		#var angle_offset: float = spread_angle * (i - (projectile_amount - 1) / 2.0) / (projectile_amount - 1)
		var angle_offset: float = spread_angle * (i - (projectile_amount / 2))
		instanced_entity.look_at(target_pos)
		instanced_entity.rotate(Vector3.UP, deg_to_rad(angle_offset))
		#var direction = -instanced_entity.global_transform.basis.z.rotated(Vector3.UP, deg_to_rad(angle_offset))
		#print(direction)
		
		instances.push_back(instanced_entity)
