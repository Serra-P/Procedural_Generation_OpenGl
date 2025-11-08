#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

// Mais quel shader manifique
void main()
{
    vec3 color = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(color, 1.0);
}
