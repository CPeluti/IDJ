class_name Projectile
extends Node3D

@export var base_speed = 20.0
@export var base_attack_damage = 1.0
var chain_amount: int = 0
var is_freezing_projectile: bool = false
var caster: CharacterBody3D
var lastHit: Array[CharacterBody3D] = []
var gm: GameMaster

func _physics_process(delta: float) -> void:
	var forward = -global_transform.basis.z
	global_position += forward * base_speed * delta

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
	
	if is_freezing_projectile:
		enemy.isFrozen = true
	
	var current_pos = self.global_position
	var closestEnemy = gm.getClosestEnemy(current_pos, 1000, lastHit)
	if closestEnemy and chain_amount > 0:
		print(closestEnemy)
		chain_amount -= 1
		var closest_enemy_pos = closestEnemy.global_position
		self.look_at(closest_enemy_pos)
		
