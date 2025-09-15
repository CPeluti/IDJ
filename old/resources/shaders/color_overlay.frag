#version 460

in vec4 color;       // vindo do vertex shader (gpu_Color)
in vec2 texCoord;    // vindo do vertex shader (gpu_TexCoord)

uniform sampler2D tex;        // textura base
uniform vec4 colorOverlay;    // cor aplicada com transparência (ex: rgba)

out vec4 FragColor;

void main(void)
{
    vec4 texColor = texture(tex, texCoord);

    // Combinação de cores:
    // 1. texColor: cor da textura
    // 2. color: cor do vértice (pode ser branca)
    // 3. colorOverlay: cor extra com alpha
    FragColor = texColor * color * colorOverlay;
}