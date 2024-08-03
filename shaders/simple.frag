#version 450 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D uTexture1;

void main()
{
FragColor = texture(uTexture1, TexCoord);
}