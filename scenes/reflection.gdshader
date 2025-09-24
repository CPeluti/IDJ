shader_type spatial;
uniform sampler2D view_texture : source_color;
void vertex() {
	// Called for every vertex the material is visible on.
}

void fragment() {
	// Called for every pixel the material is visible on.
	vec2 uv = SCREEN_UV;
	uv.x = 1.0 -uv.x;
	ALBEDO = texture(view_texture, uv).rgb;
}

//void light() {
//	// Called for every pixel for every light affecting the material.
//	// Uncomment to replace the default light processing function with this one.
//}
