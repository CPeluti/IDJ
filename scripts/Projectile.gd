extends Node3D
class_name Projectile

@export var base_speed = 20.0
@export var base_attack_damage = 1.0
var chain_amount: int
var caster: CharacterBody3D

func _physics_process(delta: float) -> void:
	var forward = -global_transform.basis.z
	global_position += forward * base_speed * delta

func getClosestEnemy():
	pass

func _on_hurtbox_area_entered(area):
	if area is HurtboxComponent and area.get_parent() != caster:
		var hurtbox : HurtboxComponent = area
		var attack = Attack.new()
		attack.attack_damage = base_attack_damage
		hurtbox.damage(attack)

		queue_free()
		
	var current_position = self.global_position
	if chain_amount > 0:
		chain_amount-=1
		#var newAngle: float = Vector2(current_position, )
		
