extends CharacterBody3D

@export var movement_speed = 60
@export var damage = 100
@export var max_health := 100

var health := max_health
var state = "idle"
var last_direction := Vector3.FORWARD
var task_queue: Array = []
var attacked := false

@onready var anim_sprite := $AnimatedSprite3D
@onready var particles := $CPUParticles3D
@onready var attack_area := $Area3D
@onready var attack_shape := $Area3D/CollisionShape3D

func _ready():
	particles.emitting = false
	attack_area.connect("body_entered", Callable(self, "_on_attack_area_body_entered"))

func _physics_process(delta):
	var velocity := Vector3.ZERO
	var direction = Vector3.ZERO

	match state:
		"idle":
			handle_idle_state(delta)
		"moving":
			velocity = handle_moving_state(delta)
		"attacking":
			handle_attacking_state(delta)
		"dying":
			handle_dying_state(delta)
		"frozen":
			handle_frozen_state(delta)

func handle_idle_state(delta):
	# Animação
	pass

func handle_moving_state(delta) -> Vector3:
	if task_queue.size() > 0:
		var command = task_queue.pop_front()
		if command.type == "move":
			last_direction = command.direction.normalized()
			look_at(global_position + last_direction, Vector3.UP)
			set_animation("walk")
			return last_direction * movement_speed
		elif command.type == "attack":
			state = "attacking"
			set_animation("attack")
	return Vector3.ZERO

func handle_attacking_state(delta):
	# Ataque
	pass

func handle_dying_state(delta):
	if !particles.emitting:
		particles.emitting = true
		set_animation("death")
		$CollisionShape3D.disabled = true
		$AttackZone/CollisionShape3D.disabled = true
		
func handle_frozen_state(delta):
	pass
		
func set_animation(action: String):
	pass

func issue_command(cmd_type: String, direction: Vector3):
	task_queue.append({"type": cmd_type, "direction": direction})

func take_damage(amount: int):
	health -= amount
	if health <= 0 and state != "dying":
		die()

func die():
	state = "dying"
	set_animation("death")

func _on_attack_area_body_entered(body):
	if state == "attacking" and !attacked and body.has_method("take_damage"):
		body.take_damage(10)
		attacked = true
