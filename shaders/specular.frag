#version 450 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 dirLightDirection;
uniform vec3 dirLightColor;

void main() {
  // Diffuse color
  vec4 texColor = texture(texture_diffuse1, TexCoords);

  vec3 color = texColor.rgb;

  // Simplified Ambient Occlusion (based on face direction)
  float ao = 1.0;
  if (Normal.z > 0.5) { // Front face
      ao = 0.6;
  } else if (Normal.z < -0.5) { // Back face
      ao = 0.4;
  } else if (Normal.x > 0.5) { // Right face
      ao = 0.5;
  } else if (Normal.x < -0.5) { // Left face
      ao = 0.5;
  } else if (Normal.y > 0.5) { // Top face
      ao = 0.8;
  } else if (Normal.y < -0.5) { // Bottom face
      ao = 0.3;
  }

  // Ambient lighting
  vec3 ambient = 0.3 * color * ao;

  // Diffuse lighting
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(dirLightDirection); // simulating sunlight
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * dirLightColor * color; // used dirLightColor instead of lightColor

  // Specular lighting
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
  
  float specularStrength = 0.15; // Specular strength can be adjusted
  vec3 specular = spec * dirLightColor * specularStrength; // used dirLightColor instead of lightColor

  vec3 result = ambient + diffuse + specular;
  FragColor = vec4(result, texColor.a);

}
