#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;
layout(location = 3) in vec2 fragUv;

layout(location = 0) out vec4 outColor;

struct PointLight {
  vec4 position;  // ignore w
  vec4 color;     // w component is intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  mat4 invView;
  vec4 ambientLightColor; // w component is intensity
  PointLight pointLighs[10];
  int lightCount;
} ubo;

layout(set = 0, binding = 1) uniform sampler2D image;

layout(push_constant) uniform Push {
  mat4 modelMatrix;   // projection * view * model
  mat4 normalMatrix;
} push;

void main() {
  vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
  vec3 surfaceNormal = normalize(fragNormalWorld);
  vec3 specularLight = vec3(0.0);

  vec3 cameraUpWorld = ubo.invView[3].xyz;
  vec3 viewDirection = normalize(cameraUpWorld - fragPosWorld);

  for (int i = 0; i < ubo.lightCount; i++) {
    PointLight light = ubo.pointLighs[i];
    vec3 directionToLight = light.position.xyz - fragPosWorld;
    float attenuation = 1.0 / dot(directionToLight, directionToLight);  // direction squared
    directionToLight = normalize(directionToLight);

    float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
    vec3 intensity = light.color.xyz * light.color.w * attenuation;

    diffuseLight += intensity * cosAngIncidence;

    //specular lighting
    vec3 halfAngle = normalize(directionToLight + viewDirection);
    float blinnTerm = dot(surfaceNormal, halfAngle);
    blinnTerm = clamp(blinnTerm, 0, 1);
    blinnTerm = pow(blinnTerm, 256.0); // higher values -> sharper highlights
    specularLight += intensity * blinnTerm;
  }

  vec3 imageColor = texture(image, fragUv).rgb;

  outColor = vec4((diffuseLight * fragColor + specularLight * fragColor) * imageColor, 1.0);
}