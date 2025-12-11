class_name FreezeProjectileSpellEffect
extends Effect

var applied = false

func _init() -> void:
	type = EffectType.Projectile

func Apply(spell: ProjectileSpell) -> void:
	if (!applied):
		spell.set_projectile_as_freezing()
	applied = true
	
func Update(delta: float) -> void:
	pass
	
func Remove() -> void:
	pass
	
func IsExpired() -> bool:
	return false
