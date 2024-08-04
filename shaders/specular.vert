#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in ivec2 aTexIndex;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
flat out ivec2 TexIndex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  TexCoords = aTexCoords;
  Normal = mat3(transpose(inverse(model))) * aNormal;
  FragPos = vec3(model * vec4(aPos, 1.0));
  TexIndex = aTexIndex;
  gl_Position = projection * view * vec4(FragPos, 1.0);
}