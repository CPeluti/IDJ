class_name CameraRig
extends Camera3D
@export var snap = true

var texel_error

@onready var _prev_rotation := global_rotation
@onready var _viewport_size := get_viewport().get_visible_rect().size
@onready var _snap_space := global_transform
# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta: float) -> void:
	if global_rotation != _prev_rotation:
		_prev_rotation = global_rotation
		_snap_space = global_transform
	var texel_size := size / _viewport_size.y
	var snap_space_position := global_position * _snap_space
	var snapped_position := snap_space_position.snapped(Vector3.ONE * texel_size)
	var snap_error := snapped_position - snap_space_position
	if snap:
		# apply camera offset as to not affect the actual transform
		h_offset = snap_error.x
		v_offset = snap_error.y
		# error in screen texels (will be used later)
		texel_error = Vector2(snap_error.x, -snap_error.y) / texel_size
	else:
		texel_error = Vector2.ZERO