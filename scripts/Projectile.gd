class_name Projectile
extends Node3D

@export var base_speed = 20.0
@export var base_attack_damage = 1.0
var chain_amount: int = 0
var caster: CharacterBody3D
var lastHit: Array[CharacterBody3D] = []
var gm: GameMaster

func increaseBaseSpeed(amount: float) -> void:
	self.base_speed += amount
	print(self.base_speed)

func _physics_process(delta: float) -> void:
	var forward = -global_transform.basis.z
	global_position += forward * base_speed * delta
	
	
func get_angle_between_positions(a: Vector3, b: Vector3) -> float:
	var dir = b - a
	var angle = atan2(dir.z, dir.x)
	var deg = rad_to_deg(angle)
	return deg + 360.0 if deg < 0.0 else deg

func _on_hurtbox_area_entered(area):
	
	var enemy: CharacterBody3D = null
	var node = area
	while node != null:
		if node is CharacterBody3D and node != caster:
			enemy = node
			break
		node = node.get_parent()
	
	if enemy == null or enemy is Player or enemy == caster:
		return
		
	if lastHit.find(enemy) >= 0:
		return
	
	lastHit.append(enemy)
	
	if area is HurtboxComponent:
		var hurtbox : HurtboxComponent = area
		var attack = Attack.new()
		attack.attack_damage = base_attack_damage
		hurtbox.damage(attack)
	
	var current_pos = self.global_position
	var closestEnemy = gm.getClosestEnemy(current_pos, 1000, lastHit)
	
	if closestEnemy and chain_amount > 0:
		chain_amount -= 1
		var closest_enemy_pos = closestEnemy.global_position
		var new_angle = get_angle_between_positions(current_pos, closest_enemy_pos)
		rotation_degrees.y = new_angle + 90
		
