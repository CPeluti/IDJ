extends CharacterBody3D

@export var speed = 80
@export var rotation_speed = 5
@export var fall_acceleration = 75
@export var cameraRig: Node3D

@export var Projectile : PackedScene
@export var target: CharacterBody3D

var target_velocity = Vector3.ZERO
var target_rotation = Vector3.ZERO


func _physics_process(delta: float) -> void:
	var direction = Vector3.ZERO
	var rotation = Vector3.ZERO
	if Input.is_action_just_pressed("attack"):
		shoot()
	if Input.is_action_pressed("move_right"):
		direction.x+=1
	if Input.is_action_pressed("move_left"):
		direction.x-=1
	if Input.is_action_pressed("move_back"):
		direction.z+=1
	if Input.is_action_pressed("move_forward"):
		direction.z-=1
	if Input.is_action_pressed("rotate_right"):
		rotation.x += 1
	if Input.is_action_pressed("rotate_left"):
		rotation.x -= 1
	if direction != Vector3.ZERO:
		direction = direction.normalized()
		$Pivot.basis = Basis.looking_at(direction)

	target_velocity.z = direction.x * speed
	target_velocity.x = direction.z * speed * -1
	if not is_on_floor():
		target_velocity.y = target_velocity.y - (fall_acceleration * delta)
	
	# cameraRig.rotate_y(rotation.x * rotation_speed * delta)
	# var rotated_speed = target_velocity.rotated(Vector3.UP, cameraRig.rotation.y)
	# cameraRig.global_translate(rotated_speed * delta * -1)
	velocity = target_velocity
	move_and_slide()

func shoot():
	var projectile:Node3D = Projectile.instantiate()
	get_parent().add_child(projectile)
	projectile.global_position = global_position
	# projectile.global_position.x += 20
	projectile.look_at(target.global_position)
