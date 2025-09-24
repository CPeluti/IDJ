shader_type spatial;

uniform vec4 foam_color : source_color = vec4(1.0,1.0,1.0,1.0);
uniform float foam_max : hint_range(0.0, 1.0, 0.01) = 0.4;
uniform float foam_min : hint_range(0.0, 1.0, 0.01) = 0.04;
uniform sampler2D normal_tex : hint_normal_roughness_texture;

uniform vec4 depth_gradient_shallow : source_color;
uniform vec4 depth_gradient_deep : source_color;
uniform float depth_maximum_distance : hint_range(0.0, 1.0, 0.1);

uniform float foam_distance : hint_range(0.0,1.0, 0.01) = 0.4;
uniform float noise_threshold : hint_range(0.0,1.0, 0.01) = 0.77;
uniform sampler2D depth_texture : hint_depth_texture, filter_linear_mipmap;
uniform sampler2D noise_texture : source_color;
uniform float u_near = 0.1;   // Near plane da câmera
uniform float u_far = 100.0;

float linearize_depth(float depth01, float near, float far) {
    return (2.0 * near * far) / (far + near - depth01 * (far - near));
}

void vertex() {
	// Called for every vertex the material is visible on.
}

void fragment() {
    vec2 uv = SCREEN_UV; // SCREEN_UV já é 0..1 na tela

    // Converte para distância linear
    //float depthLinear = linearize_depth(depth01, u_near, u_far);
	//{
		//float __depth_tex = texture(depth_tex_frg_2, SCREEN_UV).r;
		//vec4 __depth_world_pos = INV_PROJECTION_MATRIX * vec4(SCREEN_UV * 2.0 - 1.0, __depth_tex, 1.0);
		//__depth_world_pos.xyz /= __depth_world_pos.w;
		//n_out2p0 = clamp(1.0 - smoothstep(__depth_world_pos.z + n_in2p0, __depth_world_pos.z, VERTEX.z), 0.0, 1.0);
	//}

	float depth_tex = texture(depth_texture, SCREEN_UV).r;
	
	vec4 world_pos = INV_PROJECTION_MATRIX * vec4(SCREEN_UV*2.0-1.0,depth_tex,1.0);
	world_pos.xyz /= world_pos.w;
	float depth = clamp(1.0-smoothstep(world_pos.z+1.0,world_pos.z,VERTEX.z),0.0,1.0);
	//if(depth_tex>0.0){
		//ALBEDO = vec3(0.0,1.0,0.0);
	//}
	//vec4 clip_pos = VARYING_SCREEN_POS;
	//float fragmentDepthLinear = -clip_pos.z / clip_pos.w;
	//depth = 1.0- exp(-depth * 0.8);
	vec2 scale = vec2(0.3,1.0);
	vec2 uv_noise = FRAGCOORD.xy / vec2(512.0,512.0);
	uv_noise = uv_noise * scale;
	float noise_tex = texture(noise_texture, uv_noise).r;
	vec4 water_color = mix(depth_gradient_shallow, depth_gradient_deep, depth);
	
	vec3 current_normal = texture(normal_tex, uv).rgb;
	
	float normal_dot = clamp(dot(current_normal, NORMAL), 0.0, 1.0);
	
	float foamDistance = mix(foam_max, foam_min, normal_dot);
	
	float foam_depth = clamp(depth/foamDistance, 0.0, 1.0);
	float surface_noise_threshold = foam_depth * noise_threshold;
	float surface_noise = noise_tex > surface_noise_threshold ? 1.0:0.0;
	
	vec4 surface_noise_color = foam_color * surface_noise;
	ALBEDO = vec3(water_color.rgb+surface_noise_color.rgb);
	//ALBEDO = NORMAL;
	//ALPHA = 0.8;
	// Called for every pixel the material is visible on.
}

//void light() {
//	// Called for every pixel for every light affecting the material.
//	// Uncomment to replace the default light processing function with this one.
//}