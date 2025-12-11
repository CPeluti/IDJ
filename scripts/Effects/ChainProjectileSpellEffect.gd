class_name ChainProjectileSpellEffect
extends Effect

var applied = false
var chainProjectilesAmount: int = 3

func _init() -> void:
	type = EffectType.Projectile
	
func Apply(spell: ProjectileSpell) -> void:
	if (!applied):
		spell.set_chain_amount(chainProjectilesAmount)
	applied = true
	
func Update(_dt: float) -> void:
	pass

func Remove() -> void:
	pass
	
func IsExpired() -> bool:
	return false
