#pragma once
#include "GL/glew.h"
#include "stb_image.h"
class Texture
{
public:
	Texture();
	Texture(char* location);
	~Texture();

	void LoadTexture();
	void UseTexture();
	void ClearTexture();

private:
	GLuint textureId;
	int width, height, bitDepth;
	char* fileLocation;
};

