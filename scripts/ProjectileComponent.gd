extends Node3D

var base_attack_damage = 10.0
var base_speed = 10.0
func _physics_process(delta: float) -> void:
	var forward = -global_transform.basis.z
	global_position += forward * base_speed * delta


func _on_hurtbox_area_entered(area):
	if area is HurtboxComponent:
		var hurtbox : HurtboxComponent = area
		var attack = Attack.new()
		attack.attack_damage = base_attack_damage

		hurtbox.damage(attack)

		queue_free()
