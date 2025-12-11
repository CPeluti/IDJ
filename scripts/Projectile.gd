class_name Projectile
extends Node3D

@export var base_speed = 20.0
@export var base_attack_damage = 1.0
var chain_amount: int = 3
var caster: CharacterBody3D
var lastHit: Array[CharacterBody3D] = []
var gm: GameMaster

func _physics_process(delta: float) -> void:
	var forward = -global_transform.basis.z
	global_position += forward * base_speed * delta
	
func get_angle_between_positions(a: Vector3, b: Vector3) -> float:
	var dir = b - a
	var angle = atan2(dir.x, dir.z)  # XZ-plane
	var deg = rad_to_deg(angle)
	return deg + 360.0 if deg < 0.0 else deg

func _on_hurtbox_area_entered(area):
	
	var area_parent = area.get_parent()
	if area_parent is Player:
		return
		
	if lastHit.find(area_parent) >= 0:
		return
	else:
		lastHit.push_back(area_parent)
	
	if area is HurtboxComponent and area_parent != caster:
		var hurtbox : HurtboxComponent = area
		var attack = Attack.new()
		attack.attack_damage = base_attack_damage
		hurtbox.damage(attack)

		if chain_amount == 0:
			queue_free()
		
	var current_pos = self.global_position
	var closestEnemy = gm.getClosestEnemy(current_pos, 300, lastHit)
	var closest_enemy_pos = closestEnemy.position
	if chain_amount > 0:
		chain_amount-=1
		var new_angle: float = get_angle_between_positions(current_pos, closest_enemy_pos)
		self.rotate(current_pos, new_angle + 90.0)
		
