extends State
class_name EnemyAttack

@export var enemy: Enemy
@export var move_speed := 5.0
@export var attack_range := 2.0
@export var attack_cooldown := 10.0
@export var damage_amount := 50.0

var player: CharacterBody3D
var attack_timer := 0.0

func create_attack() -> Attack:
	var attack = Attack.new()
	attack.attack_damage = damage_amount
	return attack

func Enter():
	player = get_tree().get_first_node_in_group("Player")
	attack_timer = 0.0
	
func Update(delta: float):
	if player == null or enemy == null:
		return

	self.attack_timer -= delta
	if attack_timer <= 0:
		self.attack_timer = attack_cooldown
		var hurtbox := player.get_node_or_null("HurtboxComponent")
		if hurtbox == null:
			return
		var attack := create_attack()
		hurtbox.damage(attack)
	
func Physics_Update(_delta: float):
	if not is_instance_valid(player):
		Transitioned.emit(self, "Idle")
		return
	if enemy.isFrozen:
		Transitioned.emit(self, "Freeze")
		return
	
	var direction = player.global_position - enemy.global_position
	if abs(direction.length()) > attack_range:
		Transitioned.emit(self, "Follow") 
		return
	
func Exit():
	pass
