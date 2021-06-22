#ifndef GLENGINE_TEXTURE_H
#define GLENGINE_TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

class Texture {
public:
	GLuint id = 0;
	Texture(int w, int h, float* data);
	~Texture() = default;
};

class Colormap {
public:
  GLuint id = 0;
	Colormap();
	Colormap(int n, float* data);
	Colormap(const std::string &filename);
	~Colormap() = default;
	void load (int n, float* data);
	void readFromCSV(const std::string &filename);
	bool loaded = false;
};

#endif
