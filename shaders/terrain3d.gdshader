shader_type spatial;
render_mode blend_mix,depth_draw_opaque,cull_back,diffuse_burley,specular_schlick_ggx,skip_vertex_transform;

/* This is an example stripped down shader with maximum performance in mind.
 * Only Autoshader/Base/Over/Blend/Holes/Colormap are supported.
 * All terrain normal calculations take place in vetex().
 *
 * Control map indices are processed such that each ID only requires reading ONCE.
 * The following features: projection, detiling, and paintable rotation / scale
 * cannot work with this method, without the additional samples required for blending
 * between same ID textures with different values across indices.
 */

// Defined Constants
#define SKIP_PASS 0
#define VERTEX_PASS 1
#define FRAGMENT_PASS 2
#define COLOR_MAP vec4(1.0, 1.0, 1.0, 0.5)
#define DIV_255 0.003921568627450 // 1. / 255.

// Inline Functions
#define DECODE_BLEND(control) float(control >> 14u & 0xFFu) * DIV_255
#define DECODE_AUTO(control) bool(control & 0x1u)
#define DECODE_BASE(control) int(control >> 27u & 0x1Fu)
#define DECODE_OVER(control) int(control >> 22u & 0x1Fu)
#define DECODE_HOLE(control) bool(control >>2u & 0x1u)

#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
    #define fma(a, b, c) ((a) * (b) + (c))
    #define dFdxCoarse(a) dFdx(a)
    #define dFdyCoarse(a) dFdy(a)
#endif

// Toon shader
uniform sampler2D toon_noise_texture : source_color;
uniform float noise_speed : hint_range(0.0, 1.0, 0.001);
uniform float noise_scale : hint_range(0.0, 1.0, 0.001);
uniform float noise_strength : hint_range(0.0, 1.0, 0.1);
uniform float threshold : hint_range(0.0, 2.0) = 0.1;
uniform float fluffiness : hint_range(0.0, 2.0) = 0.1;

uniform vec4 toon_albedo : source_color = vec4(1.0f);
uniform sampler2D albedo_texture : source_color;
uniform bool clamp_diffuse_to_max = false;

uniform int cuts : hint_range(1, 8) = 3;
uniform float wrap : hint_range(-2.0f, 2.0f) = 0.0f;
uniform float steepness : hint_range(1.0f, 8.0f) = 1.0f;

uniform bool use_attenuation = true;

uniform bool use_specular = true;
uniform float specular_strength : hint_range(0.0f, 1.0f) = 1.0f;
uniform float specular_shininess : hint_range(0.0f, 32.0f) = 16.0f;
uniform sampler2D specular_map : source_color;

uniform bool use_rim = true;
uniform float rim_width : hint_range(0.0f, 16.0f) = 8.0f;
uniform vec4 rim_color : source_color = vec4(1.0f);

uniform bool use_ramp = false;
uniform sampler2D ramp : source_color;

uniform bool use_borders = false;
uniform float border_width = 0.01f;

varying vec3 vertex_pos;
varying vec3 normal;
varying vec3 world_pos;

float get_cloud_attenuation(vec3 _world_pos){
	vec2 uv = _world_pos.xz * noise_scale;
	vec4 noise1 = texture(toon_noise_texture, mod(uv + TIME * 2.5 * noise_speed / 10.0, 0.0));
	vec4 noise2 = texture(toon_noise_texture, mod(uv + TIME * (2.5 + fluffiness * 3.0) * noise_speed / 10.0, 0.0));
	vec4 combinedNoise = noise1 + noise2;
	float noise = combinedNoise.r;
	return mix(1.0, noise, noise_strength);
}

float split_specular(float specular) {
	return step(0.5f, specular);
}
//end toon shader

// Private uniforms
uniform vec3 _camera_pos = vec3(0.f);
uniform float _mesh_size = 48.f;
uniform uint _background_mode = 1u; // NONE = 0, FLAT = 1, NOISE = 2
uniform uint _mouse_layer = 0x80000000u; // Layer 32
uniform float _vertex_spacing = 1.0;
uniform float _vertex_density = 1.0; // = 1./_vertex_spacing
uniform float _region_size = 1024.0;
uniform float _region_texel_size = 0.0009765625; // = 1./region_size
uniform int _region_map_size = 32;
uniform int _region_map[1024];
uniform vec2 _region_locations[1024];
uniform float _texture_normal_depth_array[32];
uniform float _texture_ao_strength_array[32];
uniform float _texture_roughness_mod_array[32];
uniform float _texture_uv_scale_array[32];
uniform vec4 _texture_color_array[32];
uniform highp sampler2DArray _height_maps : repeat_disable;
uniform highp sampler2DArray _control_maps : repeat_disable;
uniform highp sampler2DArray _color_maps : source_color, filter_linear_mipmap_anisotropic, repeat_disable;
uniform highp sampler2DArray _texture_array_albedo : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform highp sampler2DArray _texture_array_normal : hint_normal, filter_linear_mipmap_anisotropic, repeat_enable;

// Public uniforms
uniform bool enable_texturing = true;
uniform float blend_sharpness : hint_range(0, 1) = 0.5;
uniform bool flat_terrain_normals = false;
// Autoshader
uniform float auto_slope : hint_range(0, 10) = 1.0;
uniform float auto_height_reduction : hint_range(0, 1) = 0.1;
uniform int auto_base_texture : hint_range(0, 31) = 0;
uniform int auto_overlay_texture : hint_range(0, 31) = 1;
// Macro Variation
uniform bool enable_macro_variation = true;
uniform vec3 macro_variation1 : source_color = vec3(1.);
uniform vec3 macro_variation2 : source_color = vec3(1.);
uniform float macro_variation_slope : hint_range(0., 1.)  = 0.333;
uniform highp sampler2D noise_texture : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform float noise1_scale : hint_range(0.001, 1.) = 0.04; // Used for macro variation 1. Scaled up 10x
uniform float noise1_angle : hint_range(0, 6.283) = 0.;
uniform vec2 noise1_offset = vec2(0.5);
uniform float noise2_scale : hint_range(0.001, 1.) = 0.076; // Used for macro variation 2. Scaled up 10x

// Varyings & Types
varying vec3 v_normal;
varying vec3 v_vertex;
varying mat3 TBN;

////////////////////////
// Vertex
////////////////////////

// Takes in world space XZ (UV) coordinates & search depth (only applicable for background mode none)
// Returns ivec3 with:
// XY: (0 to _region_size - 1) coordinates within a region
// Z: layer index used for texturearrays, -1 if not in a region
ivec3 get_index_coord(const vec2 uv, const int search) {
	vec2 r_uv = round(uv);
	vec2 o_uv = mod(r_uv,_region_size);
	ivec2 pos;
	int bounds, layer_index = -1;
	for (int i = -1; i < clamp(search, SKIP_PASS, FRAGMENT_PASS); i++) {
		if ((layer_index == -1 && _background_mode == 0u ) || i < 0) {
			r_uv -= i == -1 ? vec2(0.0) : vec2(float(o_uv.x <= o_uv.y), float(o_uv.y <= o_uv.x));
			pos = ivec2(floor((r_uv) * _region_texel_size)) + (_region_map_size / 2);
			bounds = int(uint(pos.x | pos.y) < uint(_region_map_size));
			layer_index = (_region_map[ pos.y * _region_map_size + pos.x ] * bounds - 1);
		}
	}
	return ivec3(ivec2(mod(r_uv,_region_size)), layer_index);
}

// Takes in descaled (world_space / region_size) world to region space XZ (UV2) coordinates, returns vec3 with:
// XY: (0. to 1.) coordinates within a region
// Z: layer index used for texturearrays, -1 if not in a region
vec3 get_index_uv(const vec2 uv2) {
	ivec2 pos = ivec2(floor(uv2)) + (_region_map_size / 2);
	int bounds = int(uint(pos.x | pos.y) < uint(_region_map_size));
	int layer_index = _region_map[ pos.y * _region_map_size + pos.x ] * bounds - 1;
	return vec3(uv2 - _region_locations[layer_index], float(layer_index));
}

void vertex() {
    //toon shader
    vertex_pos = VERTEX;
	normal = NORMAL;
	world_pos = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
    //end toon shader
	// Get vertex of flat plane in world coordinates and set world UV
	v_vertex = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;

	// Camera distance to vertex on flat plane
	float v_vertex_xz_dist = length(v_vertex.xz - _camera_pos.xz);

	// Geomorph vertex, set end and start for linear height interpolate
	float scale = MODEL_MATRIX[0][0];
	float vertex_lerp = smoothstep(0.55, 0.95, (v_vertex_xz_dist / scale - _mesh_size - 4.0) / (_mesh_size - 2.0));
	vec2 v_fract = fract(VERTEX.xz * 0.5) * 2.0;
	// For LOD0 morph from a regular grid to an alternating grid to align with LOD1+
	vec2 shift = (scale < _vertex_spacing + 1e-6) ? // LOD0 or not
		// Shift from regular to symetric
		mix(v_fract, vec2(v_fract.x, -v_fract.y),
			round(fract(round(mod(v_vertex.z * _vertex_density, 4.0)) *
			round(mod(v_vertex.x * _vertex_density, 4.0)) * 0.25))
			) :
		// Symetric shift
		v_fract * round((fract(v_vertex.xz * 0.25 / scale) - 0.5) * 4.0);
	vec2 start_pos = v_vertex.xz * _vertex_density;
	vec2 end_pos = (v_vertex.xz - shift * scale) * _vertex_density;
	v_vertex.xz -= shift * scale * vertex_lerp;

	// UV coordinates in world space. Values are 0 to _region_size within regions
	UV = v_vertex.xz * _vertex_density;

	// UV coordinates in region space + texel offset. Values are 0 to 1 within regions
	UV2 = fma(UV, vec2(_region_texel_size), vec2(0.5 * _region_texel_size));

	// Discard vertices for Holes. 1 lookup
	ivec3 v_region = get_index_coord(start_pos, VERTEX_PASS);
	uint control = floatBitsToUint(texelFetch(_control_maps, v_region, 0)).r;
	bool hole = DECODE_HOLE(control);

	// Show holes to all cameras except mouse camera (on exactly 1 layer)
	if ( !(CAMERA_VISIBLE_LAYERS == _mouse_layer) &&
			(hole || (_background_mode == 0u && v_region.z == -1))) {
		v_vertex.x = 0. / 0.;
	} else {
		// Set final vertex height & calculate vertex normals. 3 lookups
		ivec3 uv_a = get_index_coord(start_pos, VERTEX_PASS);
		ivec3 uv_b = get_index_coord(end_pos, VERTEX_PASS);
		float h = mix(texelFetch(_height_maps, uv_a, 0).r,texelFetch(_height_maps, uv_b, 0).r,vertex_lerp);
		v_vertex.y = h;

		// Vertex normals
		float u = mix(texelFetch(_height_maps, get_index_coord(start_pos + vec2(1,0), VERTEX_PASS), 0).r,
			texelFetch(_height_maps, get_index_coord(end_pos + vec2(1,0), VERTEX_PASS), 0).r, vertex_lerp);
		float v = mix(texelFetch(_height_maps, get_index_coord(start_pos + vec2(0,1), VERTEX_PASS), 0).r,
			texelFetch(_height_maps, get_index_coord(end_pos + vec2(0,1), VERTEX_PASS), 0).r, vertex_lerp);

		v_normal = normalize(vec3(h - u, _vertex_spacing, h - v));
	}

	// Convert model space to view space w/ skip_vertex_transform render mode
	VERTEX = (VIEW_MATRIX * vec4(v_vertex, 1.0)).xyz;

	// Apply terrain normals
	vec3 w_tangent = normalize(cross(v_normal, vec3(0.0, 0.0, 1.0)));
	vec3 w_binormal = normalize(cross(v_normal, w_tangent));
	TBN = mat3(w_tangent, w_binormal, v_normal);

	NORMAL = normalize((VIEW_MATRIX * vec4(v_normal, 0.0)).xyz);
	BINORMAL = normalize((VIEW_MATRIX * vec4(w_binormal, 0.0)).xyz);
	TANGENT = normalize((VIEW_MATRIX * vec4(w_tangent, 0.0)).xyz);
}

////////////////////////
// Fragment
////////////////////////

mat2 rotate_plane(float angle) {
	float c = cos(angle), s = sin(angle);
	return mat2(vec2(c, s), vec2(-s, c));
}

void fragment() {
	// Recover UVs
	vec2 uv = UV;
	vec2 uv2 = UV2;

	// Lookup offsets, ID and blend weight
	vec3 region_uv = get_index_uv(uv2);
	const vec3 offsets = vec3(0, 1, 2);
	vec2 index_id = floor(uv);
	vec2 weight = fract(uv);
	vec2 invert = 1.0 - weight;
	vec4 weights = vec4(
		invert.x * weight.y, // 0
		weight.x * weight.y, // 1
		weight.x * invert.y, // 2
		invert.x * invert.y  // 3
	);

	ivec3 index[4];
	// control map lookups, used for some normal lookups as well
	index[0] = get_index_coord(index_id + offsets.xy, FRAGMENT_PASS);
	index[1] = get_index_coord(index_id + offsets.yy, FRAGMENT_PASS);
	index[2] = get_index_coord(index_id + offsets.yx, FRAGMENT_PASS);
	index[3] = get_index_coord(index_id + offsets.xx, FRAGMENT_PASS);

	vec3 base_ddx = dFdxCoarse(v_vertex);
	vec3 base_ddy = dFdyCoarse(v_vertex);
	vec4 base_dd = vec4(base_ddx.xz, base_ddy.xz);
	// Calculate the effective mipmap for regionspace
	float region_mip = log2(max(length(base_ddx.xz), length(base_ddy.xz)) * _vertex_density);

	// Color map
	vec4 color_map = region_uv.z > -1.0 ? textureLod(_color_maps, region_uv, region_mip) : COLOR_MAP;

	if (flat_terrain_normals) {
		NORMAL = normalize(cross(dFdyCoarse(VERTEX),dFdxCoarse(VERTEX)));
		TANGENT = normalize(cross(NORMAL, VIEW_MATRIX[2].xyz));
		BINORMAL = normalize(cross(NORMAL, TANGENT));
	}

	// defaults
	vec4 normal_rough = vec4(0., 1., 0., 0.7);
	vec4 albedo_height = vec4(1.);
	float normal_map_depth = 1.;
	float ao_strength = 0.;

	if (enable_texturing) {
		// set to zero before accumulation
		albedo_height = vec4(0.);
		normal_rough = vec4(0.);
		normal_map_depth = 0.;
		ao_strength = 0.;
		float total_weight = 0.;
		float sharpness = fma(56., blend_sharpness, 8.);

		// Get index control data
		// 1 - 4 lookups
		uvec4 control = floatBitsToUint(vec4(
			texelFetch(_control_maps, index[0], 0).r,
			texelFetch(_control_maps, index[1], 0).r,
			texelFetch(_control_maps, index[2], 0).r,
			texelFetch(_control_maps, index[3], 0).r));

		{
			// Auto blend calculation
			float auto_blend = clamp(fma(auto_slope * 2.0, (v_normal.y - 1.0), 1.0)
				- auto_height_reduction * 0.01 * v_vertex.y, 0.0, 1.0);
			// Enable Autoshader if outside regions or painted in regions, otherwise manual painted
			uvec4 is_auto = (control & uvec4(0x1u)) | uvec4(uint(region_uv.z < 0.0));
			uint u_auto =
				((uint(auto_base_texture) & 0x1Fu) << 27u) |
				((uint(auto_overlay_texture) & 0x1Fu) << 22u) |
				((uint(fma(auto_blend, 255.0 , 0.5)) & 0xFFu) << 14u);
			control = control * (1u - is_auto) + u_auto * is_auto;
		}


		// Texture weights
		// Vectorised Deocode of all texture IDs, then swizzle to per index mapping.
		ivec4 t_id[2] = {ivec4(control >> uvec4(27u) & uvec4(0x1Fu)),
			ivec4(control >> uvec4(22u) & uvec4(0x1Fu))};
		ivec2 texture_ids[4] = ivec2[4](
			ivec2(t_id[0].x, t_id[1].x),
			ivec2(t_id[0].y, t_id[1].y),
			ivec2(t_id[0].z, t_id[1].z),
			ivec2(t_id[0].w, t_id[1].w));

		// interpolated weights.
		vec4 weights_id_1 = vec4(control >> uvec4(14u) & uvec4(0xFFu)) * DIV_255 * weights;
		vec4 weights_id_0 = weights - weights_id_1;
		vec2 t_weights[4] = {vec2(0), vec2(0), vec2(0), vec2(0)};
		for (int i = 0; i < 4; i++) {
				vec2 w_0 = vec2(weights_id_0[i]);
				vec2 w_1 = vec2(weights_id_1[i]);
				ivec2 id_0 = texture_ids[i].xx;
				ivec2 id_1 = texture_ids[i].yy;
				t_weights[0] += fma(w_0, vec2(equal(texture_ids[0], id_0)), w_1 * vec2(equal(texture_ids[0], id_1)));
				t_weights[1] += fma(w_0, vec2(equal(texture_ids[1], id_0)), w_1 * vec2(equal(texture_ids[1], id_1)));
				t_weights[2] += fma(w_0, vec2(equal(texture_ids[2], id_0)), w_1 * vec2(equal(texture_ids[2], id_1)));
				t_weights[3] += fma(w_0, vec2(equal(texture_ids[3], id_0)), w_1 * vec2(equal(texture_ids[3], id_1)));
		}


		// Process control data to determine each texture ID present, so that only
		// a single sample will be needed later, as all id are contiguous when features
		// like detiling, scale, rotation, and projection are not present.
		// 2 to 16 lookups
		uint id_read = 0u; // 1 bit per possible ID
		// world normal adjustment requires acess to previous id during next iteration
		vec4 nrm = vec4(0.0, 1.0, 0.0, 1.0);
		// adjust uv scale to account for vertex spacing
		uv *= _vertex_spacing;
		for (int i = 0; i < 4; i++) {
			for (int t = 0; t < 2; t++) {
				int id = texture_ids[i][t];
				uint mask = 1u << uint(id);
				if ((id_read & mask) == 0u) {
					// Set this id bit
					id_read |= mask;
					float id_w = t_weights[i][t];
					float id_scale = _texture_uv_scale_array[id] * 0.5;
					vec2 id_uv = fma(uv, vec2(id_scale), vec2(0.5));
					vec4 i_dd = base_dd * id_scale;
					vec4 alb = textureGrad(_texture_array_albedo, vec3(id_uv, float(id)), i_dd.xy, i_dd.zw);
					float world_normal = clamp(fma(TBN[0], vec3(nrm.x), fma(TBN[1], vec3(nrm.z), v_normal * vec3(nrm.y))).y, 0., 1.);
					nrm = textureGrad(_texture_array_normal, vec3(id_uv, float(id)), i_dd.xy, i_dd.zw);
					alb.rgb *= _texture_color_array[id].rgb;
					nrm.a = clamp(nrm.a + _texture_roughness_mod_array[id], 0., 1.);
					// Unpack normal map for blending.
					nrm.xyz = fma(nrm.xzy, vec3(2.0), vec3(-1.0));
					// height weight modifier.
					float id_weight = exp2(sharpness * log2(id_w + alb.a * world_normal));
					albedo_height += alb * id_weight;
					normal_rough += nrm * id_weight;
					normal_map_depth += _texture_normal_depth_array[id] * id_weight;
					ao_strength += _texture_ao_strength_array[id] * id_weight;
					total_weight += id_weight;
				}
			}
		}
		// normalize accumulated values back to 0.0 - 1.0 range.
		float weight_inv = 1.0 / total_weight;
		albedo_height *= weight_inv;
		normal_rough *= weight_inv;
		normal_map_depth *= weight_inv;
		ao_strength *= weight_inv;
	}

	// Macro variation. 2 lookups
	vec3 macrov = vec3(1.);
	if (enable_macro_variation) {
		float noise1 = texture(noise_texture, (uv * noise1_scale * .1 + noise1_offset) * rotate_plane(noise1_angle)).r;
		float noise2 = texture(noise_texture, uv * noise2_scale * .1).r;
		macrov = mix(macro_variation1, vec3(1.), noise1);
		macrov *= mix(macro_variation2, vec3(1.), noise2);
		macrov = mix(vec3(1.0), macrov, clamp(v_normal.y + macro_variation_slope, 0., 1.));
	}

	// Wetness/roughness modifier, converting 0 - 1 range to -1 to 1 range, clamped to Godot roughness values
	float roughness = clamp(fma(color_map.a - 0.5, 2.0, normal_rough.a), 0., 1.);

	// Apply PBR
	ALBEDO = albedo_height.rgb * color_map.rgb * macrov * toon_albedo.rgb;
	ROUGHNESS = roughness;
	SPECULAR = 1. - normal_rough.a;
	// Repack final normal map value.
	NORMAL_MAP = fma(normalize(normal_rough.xzy), vec3(0.5), vec3(0.5));
	NORMAL_MAP_DEPTH = normal_map_depth;

	// Higher and/or facing up, less occluded.
	float ao = (1. - (albedo_height.a * log(2.1 - ao_strength))) * (1. - normal_rough.y);
	AO = clamp(1. - ao * ao_strength, albedo_height.a, 1.0);
	AO_LIGHT_AFFECT = (1.0 - albedo_height.a) * clamp(normal_rough.y, 0., 1.);

}

void light() {
	// Attenuation.
	float attenuation = 1.0f;
	if (use_attenuation) {
		attenuation =  ATTENUATION * get_cloud_attenuation(world_pos);
	}
	
	// Diffuse lighting.
	float NdotL = dot(NORMAL, LIGHT);
	float diffuse_amount = NdotL + (attenuation - 1.0) + wrap;
	//float diffuse_amount = NdotL * attenuation + wrap;
	diffuse_amount *= steepness;
	float cuts_inv = 1.0f / float(cuts);
	float diffuse_stepped = clamp(diffuse_amount + mod(1.0f - diffuse_amount, cuts_inv), 0.0f, 1.0f);

	// Calculate borders.
	float border = 0.0f;
	if (use_borders) {
		float corr_border_width = length(cross(NORMAL, LIGHT)) * border_width * steepness;
		border = step(diffuse_stepped - corr_border_width, diffuse_amount)
				 - step(1.0 - corr_border_width, diffuse_amount);
	}
	
	// Apply diffuse result to different styles.
	vec3 diffuse = ALBEDO.rgb * LIGHT_COLOR / PI;
	if (use_ramp) {
		diffuse *= texture(ramp, vec2(diffuse_stepped * (1.0f - border), 0.0f)).rgb;
	} else {
		diffuse *= diffuse_stepped * (1.0f - border);
	}
	
	if (clamp_diffuse_to_max) {
		// Clamp diffuse to max for multiple light sources.
		DIFFUSE_LIGHT = max(DIFFUSE_LIGHT, diffuse);
	} else {
		DIFFUSE_LIGHT += diffuse;
	}
	
	// Specular lighting.
	if (use_specular) {
		vec3 H = normalize(LIGHT + VIEW);
		float NdotH = dot(NORMAL, H);
		float specular_amount = max(pow(NdotH, specular_shininess*specular_shininess), 0.0f)
							    * texture(specular_map, UV).r
								* attenuation;
		specular_amount = split_specular(specular_amount);
		SPECULAR_LIGHT += specular_strength * specular_amount * LIGHT_COLOR;
	}
	
	// Simple rim lighting.
	if (use_rim) {
		float NdotV = dot(NORMAL, VIEW);
		float rim_light = pow(1.0 - NdotV, rim_width);
		DIFFUSE_LIGHT += rim_light * rim_color.rgb * rim_color.a * LIGHT_COLOR / PI;
	}
}