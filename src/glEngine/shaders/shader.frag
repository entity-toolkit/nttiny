#version 330 core
in vec2 texCoord;
out vec4 FragColor;

uniform float u_time;
uniform vec2 u_mouse;
uniform vec2 u_resolution;

uniform sampler2D field;
uniform sampler1D colormap;

void main() {
  float value = texture(field, texCoord).r;
  vec4 color = texture(colormap, value);
  FragColor = vec4(color.rgb, 1.0);
}
