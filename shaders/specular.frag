#version 450 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
flat in ivec2 TexIndex;

uniform sampler2D texture_diffuse1;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 dirLightDirection;
uniform vec3 dirLightColor;

const float atlasWidth = 64.0;
const float atlasHeight = 64.0;
const float spriteWidth = 16.0;
const float spriteHeight = 16.0;

void main() {
  float startX = TexIndex.x * spriteWidth;
  float startY = TexIndex.y * spriteHeight;
  float uMin = startX / atlasWidth;
  float vMin = startY / atlasHeight;
  float uMax = (startX + spriteWidth) / atlasWidth;
  float vMax = (startY + spriteHeight) / atlasHeight;
  
  vec2 adjustedTexCoords = vec2(
    mix(uMin, uMax, TexCoords.x),
    mix(vMin, vMax, TexCoords.y)
  );

  // Diffuse color
  vec3 color = texture(texture_diffuse1, adjustedTexCoords).rgb;

  // Ambient lighting
  vec3 ambient = 0.3 * color;

  // Diffuse lighting
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(dirLightDirection); // simulating sunlight
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * dirLightColor * color; // used dirLightColor instead of lightColor

  // Specular lighting
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
  
  float specularStrength = 0.5; // Specular strength can be adjusted
  vec3 specular = spec * dirLightColor * specularStrength; // used dirLightColor instead of lightColor

  vec3 result = ambient + diffuse + specular;
  FragColor = vec4(result, 1.0);
}