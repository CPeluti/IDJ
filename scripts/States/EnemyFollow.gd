extends State
class_name EnemyFollow

@export var enemy: CharacterBody3D
@export var move_speed := 5.0

var player: CharacterBody3D

func Enter():
	player = get_tree().get_first_node_in_group("Player")

func Update(delta: float):
	pass

func Physics_Update(delta: float):
	var direction = player.global_position - enemy.global_position

	if direction.length() > 5:
		enemy.velocity = direction.normalized() * move_speed
	else:
		enemy.velocity = Vector3()

	if direction.length() > 30:
		Transitioned.emit(self, "Idle")

func Exit():
	pass
