#version 330 core
in vec2 texCoord;
out vec4 FragColor;

uniform float u_time;
uniform vec2 u_mouse;
uniform vec2 u_resolution;

uniform sampler2D field;

void main(){
  float color = texture(field, texCoord).r;
  FragColor = vec4(vec3(color), 1.0);
}
