extends CharacterBody3D
class_name Player

enum Mode{
	Cast,
	Normal
}

@export var speed: float = 8
@export var rotation_speed: float = 5
@export var fall_acceleration: float = 75
@export var cameraRig: Node3D

@export var textRenderer: Node3D

@export var target: CharacterBody3D

var mode: Mode = Mode.Cast

var castString = "teste"
@export var gm: GameMaster

var casted_spells: Array[SpellStrategy]= []

var target_velocity = Vector3.ZERO
var target_rotation = Vector3.ZERO

var direction = Vector3.ZERO

var projectile = preload("res://scenes/Projectile.tscn")

func _physics_process(delta: float) -> void:
	textRenderer.set_text(castString)
	var _rotation = Vector3.ZERO
	
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
	
	if direction != Vector3.ZERO:
		direction = direction.normalized()
		$Pivot.basis = Basis.looking_at(direction)

	target_velocity.z = direction.x * speed
	target_velocity.x = direction.z * speed * -1
	if not is_on_floor():
		target_velocity.y = target_velocity.y - (self.fall_acceleration * delta)
	
	# cameraRig.rotate_y(rotation.x * rotation_speed * delta)
	# var rotated_speed = target_velocity.rotated(Vector3.UP, cameraRig.rotation.y)
	# cameraRig.global_translate(rotated_speed * delta * -1)
	velocity = target_velocity
	move_and_slide()
	direction = Vector3.ZERO
	
	cameraRig.global_position = self.global_position

func _input(event: InputEvent) -> void:
	if event is not InputEventMouseMotion:
		
			
		if event.keycode == KEY_BACKSPACE and event.pressed:
			castString = castString.substr(0, castString.length() - 1)
		if event.keycode == KEY_ENTER and event.pressed:
			castString = ""
			mode = Mode.Normal
		if event is InputEventKey and event.pressed and event.unicode > 0:
			var c = char(event.unicode)
			castString += c

func shoot():
	var effect = MoreProjectileSpellEffect.new()
	var chainEffect = ChainProjectileSpellEffect.new()
	var fasterProjectileEffect = FasterProjectileEffect.new()
	var freezeProjectileEffect = FreezeProjectileSpellEffect.new()
	
	var projectile_spell = ProjectileSpell.new(self, projectile, gm)
	projectile_spell.effects.append(effect)
	projectile_spell.effects.append(chainEffect)
	projectile_spell.effects.append(fasterProjectileEffect)
	projectile_spell.effects.append(freezeProjectileEffect)
	casted_spells.push_back(projectile_spell)

	var closestEnemy: CharacterBody3D = gm.getClosestEnemy(self.global_position, 1000)
	if (closestEnemy):
		projectile_spell.cast_spell(closestEnemy.global_position)
