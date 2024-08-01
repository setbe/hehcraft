#version 450 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_ao;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
  // Diffuse color
  vec3 color = texture(texture_diffuse1, TexCoords).rgb;

  // Ambient Occlusion
  float ao = texture(texture_ao, TexCoords).r;

  // Ambient lighting
  vec3 ambient = 0.3 * color * ao;

  // Diffuse lighting
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColor * color;

  // Specular lighting
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = vec3(0.3) * spec * lightColor;

  vec3 result = ambient + diffuse + specular;
  FragColor = vec4(result, 1.0);
}
