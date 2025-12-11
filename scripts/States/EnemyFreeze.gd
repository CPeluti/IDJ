class_name EnemyFreeze
extends State

@export var enemy: Enemy
var freeze_timer: float = 5.0

func reset_timer():
	freeze_timer = 5.0

func Enter():
	pass

func Update(delta: float):
	if freeze_timer > 0:
		freeze_timer -= delta
	else:
		reset_timer()
		Transitioned.emit(self, "Idle")

func Physics_Update(delta: float):
	enemy.velocity = Vector3(0, 0, 0)

func Exit():
	enemy.isFrozen = false
