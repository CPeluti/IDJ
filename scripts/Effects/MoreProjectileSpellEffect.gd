class_name MoreProjectileSpellEffect
extends Effect

var applied = false
var extraProjectilesAmount: int = 3

func _init() -> void:
	type = EffectType.Projectile

func Apply(spell: ProjectileSpell) -> void:
	if (!applied):
		spell.increase_projectile_amount(extraProjectilesAmount)
	applied = true
	
func Remove() -> void:
	pass
	
func Update(dt: float) -> void:
	pass
	
func IsExpired() -> bool:
	return false
	
