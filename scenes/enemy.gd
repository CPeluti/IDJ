class_name Enemy
extends CharacterBody3D

var isFrozen: bool = false

func _physics_process(delta: float) -> void:
	move_and_slide()
