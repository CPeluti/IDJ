shader_type spatial;
//render_mode diffuse_toon;

uniform vec3 albedo : source_color = vec3(1.0, 1.0, 1.0);

uniform float brightness_compensation : hint_range(0.0, 2.0) = 1.14;

void vertex() {
    MODELVIEW_MATRIX = VIEW_MATRIX * mat4(
        INV_VIEW_MATRIX[0],
        INV_VIEW_MATRIX[1],
        INV_VIEW_MATRIX[2],
        MODEL_MATRIX[3]
    );
    MODELVIEW_NORMAL_MATRIX = mat3(MODELVIEW_MATRIX);
    
}

void fragment() {
    ALBEDO = albedo;
    //NORMAL = vec3(0.0, 1.0, 0.0);
    
}

void light() {
	// --- CLOUD SHADOW UNIFORM BLADE SHADING ---
	// Instead of using world_pos (which varies per fragment),
	// we use lighting_sample_position (blade center) for uniform shading
	float attenuation = 1.0f;
		// KEY CHANGE: Use lighting_sample_position instead of world_pos
		// This samples the cloud noise at the blade's center position only
		// Result: Same shadow value applied to ALL fragments of this blade
	attenuation = ATTENUATION;

	// Standard cel-shading diffuse lighting calculations
	float NdotL = dot(NORMAL, LIGHT);
	float diffuse_amount = NdotL + (attenuation - 1.0);
	float cuts_inv = 1.0f / float(3);

	//float current_cut = diffuse_amount + mod(1.0f - diffuse_amount, cuts_inv);
	float closest_cut_number = floor(diffuse_amount/cuts_inv+0.5);
	float closest_cut = closest_cut_number*cuts_inv;


	//for(int i = 0; i<=cuts; ++i){
		//int cut = max(0, i + cut_mod);
		//float val = 1.0 -  (float(cut)/float(cuts));
		//diffuse_stepped += smoothstep(val - threshold_gradient_size, val, diffuse_amount);
	//}
	//diffuse_stepped/=float(cuts);
	// Calculate borders for cel-shading outline effects


	// Apply diffuse result with optional ramp texture
	vec3 diffuse = ALBEDO.rgb * LIGHT_COLOR / PI;


	// Accumulate or clamp diffuse lighting

	DIFFUSE_LIGHT += diffuse;

	// Specular lighting (also uses the uniform attenuation for consistency)



}