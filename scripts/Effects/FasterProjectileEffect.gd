class_name FasterProjectileEffect
extends Effect

var applied = false
var speed_increase: float = 50.0

func _init() -> void:
	type = EffectType.Projectile

func Apply(spell: ProjectileSpell) -> void:
	if (!applied):
		spell.increase_projectile_speed(speed_increase)
	applied = true
	
func Update(delta: float) -> void:
	pass
	
func Remove() -> void:
	pass
	
func IsExpired() -> bool:
	return false
