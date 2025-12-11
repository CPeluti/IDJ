extends State
class_name EnemyFollow

@export var enemy: CharacterBody3D
@export var move_speed := 5.0

var player: CharacterBody3D

func Enter():
	player = get_tree().get_first_node_in_group("Player")

func Update(_delta: float):
	pass

func Physics_Update(_delta: float):
	if not is_instance_valid(player):
		Transitioned.emit(self, "Idle")
		return
	
	var direction = player.global_position - enemy.global_position

	if direction.length() > 3:
		enemy.velocity = direction.normalized() * move_speed

	if abs(direction.length()) > 10:
		Transitioned.emit(self, "Idle")
		
	if abs(direction.length()) <= 2:
		Transitioned.emit(self, "Attack")

func Exit():
	pass
