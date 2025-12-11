@abstract
class_name Effect

enum EffectType {
	Projectile,
	Area,
	Entity
}

var type: EffectType

func setEffectType(newType: EffectType) -> void:
	type = newType

func getEffectType() -> EffectType:
	return type

@abstract
func Apply(spell) -> void

@abstract
func Remove() -> void
	
@abstract
func IsExpired() -> bool
	
@abstract
func Update(_dt: float) -> void
