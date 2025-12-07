extends Node

@export var lifetime: float = 1.0
var current_lifetime: float = 0.0

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	current_lifetime += delta
	if(current_lifetime>=lifetime):
		#todo fazer um sistema para object pooling
		get_parent().free()
	pass
