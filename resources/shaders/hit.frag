#version 460

in vec4 color;
in vec2 texCoord;

uniform sampler2D tex0;

out vec4 FragColor;

void main()
{
    vec4 texColor = texture(tex0, texCoord);
    
    // Usa o alpha da textura como máscara
    if (texColor.a < 0.01) {
        discard; // ignora o pixel totalmente transparente
    }

    // Mostra branco onde a textura é visível
    FragColor = vec4(1.0, 1.0, 1.0, texColor.a);
}