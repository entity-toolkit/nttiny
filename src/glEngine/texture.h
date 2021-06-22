#ifndef GLENGINE_TEXTURE_H
#define GLENGINE_TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Texture {
public:
	GLuint id = 0;
	Texture(int w, int h, float* data);
	~Texture() = default;
};

class Colormap {
public:
  GLuint id = 0;
	Colormap(int n, float* data);
	~Colormap() = default;
};

#endif
