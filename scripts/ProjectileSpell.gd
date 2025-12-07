class_name ProjectileSpell
extends SpellStrategy

var mana_cost : float
var speed : float
var damage : float

var instances : Array[Node3D]

var owner: Node3D
var starting_position: Vector3

var modifiers : Array[String]
var projectile: PackedScene

var spell_element : SpellElement = SpellElement.fire
var base_damage = 10.0
var base_speed = 30.0
var projectile_amount : int = 3
var base_mana_cost : float = 10.0

func _init(owner: Node3D, projectile: PackedScene) -> void:
	self.owner = owner
	self.projectile = projectile

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
func ApplyEffects() -> void:
	pass
func set_projectile_amount(count: int) -> void:
	projectile_amount = count
func set_mana_cost(factor: float) -> void:
	mana_cost = factor
func update_effect(_dt: float):
	pass
func remove_effect(_effect_name: String):
	pass
func cast_spell(target_pos: Vector3) -> void:
	for i in projectile_amount:
		var instanced_entity: Node3D = projectile.instantiate()
		owner.get_parent().add_child(instanced_entity)
		instanced_entity.global_position = owner.global_position
		
		var spread_angle = 10.0
		var angle_offset: float = spread_angle * (i - (projectile_amount - 1) / 2.0) / (projectile_amount - 1)
		instanced_entity.look_at(target_pos)
		instanced_entity.rotate(Vector3.UP, angle_offset)
		#var direction = -instanced_entity.global_transform.basis.z.rotated(Vector3.UP, deg_to_rad(angle_offset))
		#print(direction)
		
		instances.push_back(instanced_entity)
