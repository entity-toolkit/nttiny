#version 330 core
in vec2 position;
out vec2 texCoord;

void main() {
  texCoord = (position + 1.0f) * 0.5f;
  gl_Position = vec4(position, 0.0, 1.0);
}
