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

uniform vec3 dirLightDirection;
uniform vec3 dirLightColor;

void main() {
  // Diffuse color
  vec3 color = texture(texture_diffuse1, TexCoords).rgb;

  // Ambient Occlusion
  float ao = texture(texture_ao, TexCoords).r;

  // Ambient lighting
  vec3 ambient = 0.3 * color * ao;

  // Diffuse lighting
  vec3 norm = normalize(Normal);
  // vec3 lightDir = normalize(lightPos - FragPos);
  vec3 lightDir = normalize(dirLightDirection); // simulating sunlight
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColor * color;

  // Specular lighting
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  
  float specularStrength = length(pow(color, vec3(2.0)));
  vec3 specular = spec * lightColor * specularStrength;
  

  vec3 result = ambient + diffuse + specular;
  vec3 debugColor = vec3(TexCoords, 0.0);

  FragColor = vec4(debugColor, 1.0);
}
