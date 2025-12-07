extends CharacterBody3D
class_name Enemy

func _physics_process(delta: float) -> void:
	move_and_slide()
