shader_type spatial;
render_mode blend_mix, depth_prepass_alpha, depth_draw_opaque;
//render_mode ambient_light_disabled;

// --- Cloud Shadows and Noise Configuration ---
uniform sampler2D noise_texture : source_color;
uniform float noise_speed : hint_range(0.0, 1.0, 0.001);
uniform float noise_scale : hint_range(0.0, 1.0, 0.001);
uniform float noise_strength : hint_range(0.0, 1.0, 0.1);
uniform float threshold : hint_range(0.0, 2.0) = 0.1;
uniform float fluffiness : hint_range(0.0, 2.0) = 0.1;

// --- Basic Material Properties ---
uniform sampler2D albedo_texture:source_color,filter_nearest;
uniform vec4 albedo : source_color;
uniform float alpha_scissor_threshold : hint_range(0.0, 1.0, 0.001);
uniform bool clamp_diffuse_to_max = false;

// --- Cel Shading Properties ---
uniform int cuts : hint_range(1, 8) = 3;
uniform float wrap : hint_range(-2.0f, 2.0f) = 0.0f;
uniform float steepness : hint_range(1.0f, 8.0f) = 1.0f;
uniform bool use_attenuation = true;
uniform float threshold_gradient_size : hint_range(0.001, 1.0) = 0.15;

// --- Specular Lighting ---
uniform bool use_specular = true;
uniform float specular_strength : hint_range(0.0f, 1.0f) = 1.0f;
uniform float specular_shininess : hint_range(0.0f, 32.0f) = 16.0f;
uniform sampler2D specular_map : source_color;

// --- Rim Lighting ---
uniform bool use_rim = true;
uniform float rim_width : hint_range(0.0f, 16.0f) = 8.0f;
uniform vec4 rim_color : source_color = vec4(1.0f);

// --- Advanced Rendering Options ---
uniform bool use_ramp = false;
uniform sampler2D ramp : source_color;
uniform bool use_borders = false;
uniform float border_width = 0.01f;

// --- FPS-Based Rotation Animation ---
// Toggle to enable/disable the rotation animation
uniform bool use_fps_rotation = false;
// Maximum pitch angle the grass blade can tilt (in degrees)
uniform float rotation_strength : hint_range(0.0, 90.0) = 15.0;
// Base animation speed multiplier
uniform float rotation_speed : hint_range(0.1, 10.0) = 1.0;
// Animation frames per second (controls the steppiness of the animation)
uniform float rotation_fps : hint_range(0.1, 60.0) = 10.0;

// --- Varying Variables ---
varying vec3 vertex_pos;
varying vec3 normal;
varying vec3 world_pos;
varying vec3 modelOrigin;
varying vec3 instance_world_origin_var;
// KEY: This single position ensures uniform lighting across the entire blade
varying vec3 lighting_sample_position;

varying int cut_mod;

// --- Utility Functions ---
float split_specular(float specular) {
	return step(0.5f, specular);
}

// Cloud attenuation function - samples noise at a specific world position
// This is used for both per-pixel effects and uniform blade shading

float quantizeValue(float input_value, int levels, float smoothness) {

    // Convert to discrete levels
    float step_size = 1.0 / float(levels - 1);
    float scaled = input_value * float(levels - 1);
    float lower = floor(scaled);
    float upper = ceil(scaled);

    // Calculate quantized levels
    float lower_level = lower / float(levels - 1);
    float upper_level = upper / float(levels - 1);

    if (smoothness <= 0.0) {
        // Hard quantization
        return round(scaled) / float(levels - 1);
    } else {
        // Smooth quantization with transitions
        float fraction = scaled - lower;
        float smooth_fraction = smoothstep(0.5 - smoothness, 0.5 + smoothness, fraction);
        return mix(lower_level, upper_level, smooth_fraction);
    }
}

float get_cloud_attenuation(vec3 _world_pos){
	vec2 uv = _world_pos.xz * noise_scale;
	vec4 noise1 = texture(noise_texture, mod(uv + TIME * 2.5 * noise_speed / 10.0, 0.0));
	vec4 noise2 = texture(noise_texture, mod(uv + TIME * (2.5 + fluffiness * 3.0) * noise_speed / 10.0, 0.0));
	vec4 combinedNoise = noise1 + noise2;
	float noise = combinedNoise.r * noise_strength;
	float cloud_lighting_mask = smoothstep(0.0, 1.0, noise);
	//cloud_lighting_mask = quantizeValue(cloud_lighting_mask, cuts, color_variation_softness);
	return mix(1.0, noise, noise_strength);
}

// Helper function to create pitch rotation matrix around Z axis (sideways tilting)
// This rotates the grass blade to simulate wind bending
mat3 rotate_z(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat3(
        vec3(c, -s, 0.0),
        vec3(s, c, 0.0),
        vec3(0.0, 0.0, 1.0)
    );
}

void vertex() {
	// Store the original vertex position before any transformations
	vec3 original_vertex = VERTEX;

	// Set up billboard transformation matrix to face the camera
	mat4 modified_model_view = VIEW_MATRIX * mat4(
        vec4(INV_VIEW_MATRIX[0].xyz, 0.0),
        vec4(INV_VIEW_MATRIX[1].xyz, 0.0),
        vec4(INV_VIEW_MATRIX[2].xyz, 0.0),
        MODEL_MATRIX[3]
    );

	mat4 initial_model_matrix = MODEL_MATRIX;

	// CLOUD SHADOW KEY SETUP: Store the instance center position
	// This is the world position of the grass blade's center/origin
    instance_world_origin_var = initial_model_matrix[3].xyz;

	// Store various positions for fragment shader use
	vertex_pos = VERTEX;
	normal = NORMAL;
	modelOrigin = (MODEL_MATRIX * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
	world_pos = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;

	// --- FPS-BASED ROTATION ANIMATION ---
	if (use_fps_rotation) {
		// Sample noise using the instance center position for consistent phase per blade
		// This ensures each blade has a different animation phase but remains consistent
		vec2 noise_sample_coord = instance_world_origin_var.xz * noise_scale;
		float rotation_noise = texture(noise_texture, noise_sample_coord).r;

		// Create a phase offset (0 to 2π) so each blade starts animation at different times
		float phase_offset = rotation_noise * 6.28318; // Full 2π range

		// Create FPS-based time stepping
		// This quantizes the animation time to create discrete animation frames
		float base_time = TIME * rotation_speed + phase_offset;
		float animated_time = floor(base_time * rotation_fps) / rotation_fps;

		// Create back-and-forth oscillation using sine wave
		float oscillation = sin(animated_time * 6.28318);

		// Calculate the pitch angle based on oscillation and strength
		float pitch_angle = oscillation * radians(rotation_strength);

		// Apply wind factor based on vertex height (only affect top portions)
		float wind_factor = clamp(original_vertex.y, 0.0, 1.0);
		pitch_angle *= wind_factor;

		// Apply pitch rotation around the bottom center of the blade
		// This creates realistic bending motion from the base
		vec3 pivot_point = vec3(original_vertex.x, 0.0, original_vertex.z);

		// Translate vertex to pivot, rotate, then translate back
		vec3 relative_vertex = original_vertex - pivot_point;
		mat3 pitch_matrix = rotate_z(pitch_angle);
		relative_vertex = pitch_matrix * relative_vertex;
		VERTEX = relative_vertex + pivot_point;
	}

    MODELVIEW_MATRIX = modified_model_view;

	// CLOUD SHADOW CRITICAL SETUP: Set the lighting sample position to blade center
	// This ensures ALL lighting calculations (including cloud shadows) use the same point
	// Result: Uniform shading across the entire grass blade geometry
	lighting_sample_position = instance_world_origin_var;
}

void fragment() {
	cut_mod = 0;
	// Standard material setup
	vec4 albedo_tex = texture(albedo_texture, UV);
	ALBEDO = albedo.rgb * albedo_tex.rgb;

	// CLOUD SHADOW KEY: Override the lighting vertex position
	// This forces all lighting calculations to use the blade's center position
	// instead of individual fragment positions
	LIGHT_VERTEX = (VIEW_MATRIX * vec4(lighting_sample_position, 1.0)).xyz;

	ALPHA *= albedo.a * albedo_tex.a;
	ALPHA_SCISSOR_THRESHOLD = alpha_scissor_threshold;
}

void light() {
	// --- CLOUD SHADOW UNIFORM BLADE SHADING ---
	// Instead of using world_pos (which varies per fragment),
	// we use lighting_sample_position (blade center) for uniform shading
	float attenuation = 1.0f;
	if (use_attenuation) {
		// KEY CHANGE: Use lighting_sample_position instead of world_pos
		// This samples the cloud noise at the blade's center position only
		// Result: Same shadow value applied to ALL fragments of this blade
		attenuation = ATTENUATION * get_cloud_attenuation(lighting_sample_position);
	}
	float diffuse_stepped;

	// Standard cel-shading diffuse lighting calculations
	float NdotL = dot(NORMAL, LIGHT);
	float diffuse_amount = NdotL + (attenuation - 1.0) + wrap;
	diffuse_amount *= steepness;
	float cuts_inv = 1.0f / float(cuts);

	diffuse_stepped = clamp(diffuse_amount + mod(1.0f - diffuse_amount, cuts_inv), 0.0f, 1.0f);
	//float current_cut = diffuse_amount + mod(1.0f - diffuse_amount, cuts_inv);
	float closest_cut_number = floor(diffuse_amount/cuts_inv+0.5);
	float closest_cut = closest_cut_number*cuts_inv;
	float cut_half_width = 0.5 * cuts_inv * threshold_gradient_size;

	float low = max(0.0, closest_cut-cut_half_width);
	float high = min(1.0, closest_cut+cut_half_width);
	if(diffuse_amount>=low && diffuse_amount<=high ){
		float amount = smoothstep(low,high,diffuse_amount);
		float bottom_value = clamp(low + mod(1.0f - low, cuts_inv), 0.0f, 1.0f);
		float upper_value = clamp(high + mod(1.0f - high, cuts_inv), 0.0f, 1.0f);
		diffuse_stepped = mix(bottom_value, upper_value, amount);
	}
	//for(int i = 0; i<=cuts; ++i){
		//int cut = max(0, i + cut_mod);
		//float val = 1.0 -  (float(cut)/float(cuts));
		//diffuse_stepped += smoothstep(val - threshold_gradient_size, val, diffuse_amount);
	//}
	//diffuse_stepped/=float(cuts);
	// Calculate borders for cel-shading outline effects
	float border = 0.0f;
	if (use_borders) {
		float corr_border_width = length(cross(NORMAL, LIGHT)) * border_width * steepness;
		border = step(diffuse_stepped - corr_border_width, diffuse_amount)
				 - step(1.0 - corr_border_width, diffuse_amount);
	}

	// Apply diffuse result with optional ramp texture
	vec3 diffuse = ALBEDO.rgb * LIGHT_COLOR / PI;
	if (use_ramp) {
		diffuse *= texture(ramp, vec2(diffuse_stepped * (1.0f - border), 0.0f)).rgb;
	} else {
		diffuse *= diffuse_stepped * (1.0f - border);
	}

	// Accumulate or clamp diffuse lighting
	if (clamp_diffuse_to_max) {
		// Clamp diffuse to max for multiple light sources.
		DIFFUSE_LIGHT = max(DIFFUSE_LIGHT, diffuse);
	} else {
		DIFFUSE_LIGHT += diffuse;
	}

	// Specular lighting (also uses the uniform attenuation for consistency)
	if (use_specular) {
		vec3 H = normalize(LIGHT + VIEW);
		float NdotH = dot(NORMAL, H);
		float specular_amount = max(pow(NdotH, specular_shininess*specular_shininess), 0.0f)
							    * texture(specular_map, UV).r
								* attenuation; // Same attenuation = uniform specular shading
		specular_amount = split_specular(specular_amount);
		SPECULAR_LIGHT += specular_strength * specular_amount * LIGHT_COLOR;
	}

	// Rim lighting (also uses the uniform attenuation)
	if (use_rim) {
		float NdotV = dot(NORMAL, VIEW);
		float rim_light = pow(1.0 - NdotV, rim_width);
		// Apply the same attenuation to rim lighting for consistency
		DIFFUSE_LIGHT += rim_light * rim_color.rgb * rim_color.a * LIGHT_COLOR / PI * attenuation;
	}
}