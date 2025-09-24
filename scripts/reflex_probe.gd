extends SnappingCamera

@export var player_camera: Camera3D
@export var player: Node3D
@export var water_body: MeshInstance3D
# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	# global_position = player_camera.global_position 
	# print(self.transform.basis.z)
	print(water_body.transform.basis.y.normalized())
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta: float) -> void:
	var forward_vector = -player_camera.global_transform.basis.z
	print(forward_vector)
	var up_vector = player_camera.global_transform.basis.y

	var normal = Vector3.UP
	var projection_length = normal.dot(player_camera.global_position-water_body.global_position)


	# var water_plane := Plane(Vector3.UP, -water_body.global_position.y)
	# var projection_length = water_plane.distance_to(player_camera.global_position)+water_plane.d
	var proj = -normal*projection_length
	var camera_pos = player_camera.global_position + (2*proj)
	global_position = camera_pos
	
	var probe_forward = forward_vector.bounce(normal)
	var probe_up = up_vector.bounce(normal)
	look_at(camera_pos+probe_forward, probe_up)


	# var mirrored_pos : Vector3 = player_camera.transform.origin
	# mirrored_pos.y = water_body.global_position.y - (mirrored_pos.y - water_body.global_position.y)

	# var reflect : Basis = Basis(
	# 	Vector3(1,  0,  0),
	# 	Vector3(0, -1,  0),
	# 	Vector3(0,  0,  1)
	# )

	# var mirrored_basis : Basis = reflect * player_camera.basis

	# global_transform = Transform3D(mirrored_basis, mirrored_pos)


	var inv_view = global_transform.affine_inverse()
	var normal4 = inv_view.basis * normal
	var point4 = inv_view*water_body.global_position
	var d = -normal4.dot(point4)
	
	
	custom_projection = CalculateObliqueMatrixOrtho(Vector4(normal4.x, normal4.y, normal4.z, d))

	pass

func CalculateObliqueMatrixOrtho(clipPlane: Vector4):
	var new_projection: Projection = self.custom_projection
	var q : Vector4 = new_projection.inverse() * Vector4(sign(clipPlane.x),sign(clipPlane.y),1.0,1.0)
	var c : Vector4 = clipPlane * (2.0 / clipPlane.dot(q))
	# print(c)
	new_projection[0][2] = c.x
	new_projection[1][2] = c.y
	new_projection[2][2] = c.z
	new_projection [3][2] = c.w - 1.0
	return new_projection
