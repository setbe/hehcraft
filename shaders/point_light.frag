#version 450

layout(location = 0) in vec2 fragOffset;

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

layout(push_constant) uniform Push {
  vec4 position;
  vec4 color;
  float radius;
} push;

const float M_PI = 3.14159265359;

void main() {
  float dis = sqrt(dot(fragOffset, fragOffset));
  if (dis >= 1.0)
  {
    discard;
  }
  // outColor = vec4(push.color.xyz, 0.5 * (cos(dis * M_PI) + 1.0));

  float cosDis = 0.5 * (cos(dis * M_PI) + 1.0);
  outColor = vec4(push.color.xyz + cosDis, cosDis);
}

