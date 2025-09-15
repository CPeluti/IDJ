#version 460
precision mediump float;
in vec4 color;
in vec2 texCoord;


uniform sampler2D tex;
uniform vec4 myColor;

out vec4 FragColor;

void main(){
    // float outline_size = 0.0;
    float progress = 1;
    vec3 outline_color = vec3(1.0, 0.0, 0.0);
    float width = 0.0025;
    vec4 color = texture(tex, texCoord);

    // float alpha_threshold = step(sprite_color.a, 1.0 - outline_size) - step(sprite_color.a, 1.0);

    // vec4 final_color = mix(sprite_color, outline, alpha_threshold);

    // gl_FragColor = final_color;
	vec4 line_neg_y = vec4(outline_color.rgb, texture(tex, vec2(texCoord.x, texCoord.y - width)).a);
	vec4 line_pos_y = vec4(outline_color.rgb, texture(tex, vec2(texCoord.x, texCoord.y + width)).a);
	vec4 line_neg_x = vec4(outline_color.rgb, texture(tex, vec2(texCoord.x - width, texCoord.y)).a);
	vec4 line_pos_x = vec4(outline_color.rgb, texture(tex, vec2(texCoord.x + width, texCoord.y)).a);
	vec4 outline_y = mix(line_neg_y, line_pos_y, line_pos_y);
	vec4 outline_x = mix(line_neg_x, line_pos_x, line_pos_x);
	vec4 outline = mix(outline_y, outline_x, outline_x);
	FragColor.rgba = mix(color, outline, progress * (1.0 - color.a));
}