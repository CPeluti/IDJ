extends State
class_name EnemyIdle

@export var enemy: CharacterBody3D
@export var move_speed := 5.0

var player: CharacterBody3D
var move_direction: Vector3
var wander_time: float

func randomize_wander():
	move_direction = Vector3(randf_range(-1, 1), 0, randf_range(-1, 1)).normalized()
	wander_time = randf_range(1, 3)

func Enter():
	player = get_tree().get_first_node_in_group("Player")
	randomize_wander()

func Update(delta: float):
	if wander_time > 0:
		wander_time -= delta
	else:
		randomize_wander()

func Physics_Update(delta: float):
	if not is_instance_valid(player):
		Transitioned.emit(self, "Idle")
		return
	
	if enemy:
		enemy.velocity = move_direction * move_speed
	
	var direction = player.global_position - enemy.global_position
	if direction.length() <= 20:
		Transitioned.emit(self, "Attack")
	elif direction.length() <= 30:
		Transitioned.emit(self, "Follow")

func Exit():
	pass
