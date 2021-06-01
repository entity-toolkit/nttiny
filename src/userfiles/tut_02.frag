#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main() {
  FragColor = vec4(0.0, texture(ourTexture, TexCoord).r, 0.0, 1.0);
}

