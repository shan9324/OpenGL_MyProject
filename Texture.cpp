#include "Texture.h"
Texture::Texture(){
	this->textureId = 0;
	this->width= 0;
	this->height = 0;
	this->bitDepth = 0;
	this->fileLocation = const_cast<char*>("");
}
Texture::Texture(char* location){
	this->textureId = 0;
	this->width = 0;
	this->height = 0;
	this->bitDepth = 0;
	this->fileLocation = const_cast<char*>(location);
}
Texture::~Texture(){
	this->ClearTexture();
}

void Texture::LoadTexture(){
	unsigned char* texData = stbi_load(this->fileLocation, &(this->width), &(this->height), &(this->bitDepth), 0);
	if (!texData)
		throw "Failed to find texture data";

	glGenTextures(1, &(this->textureId));
	glBindTexture(GL_TEXTURE_2D, this->textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Get the data for our image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(texData);
}
void Texture::UseTexture(){

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textureId);
}
void Texture::ClearTexture(){
	
	glDeleteTextures(1, &(this->textureId));
	this->textureId = 0;
	this->width = 0;
	this->height = 0;
	this->bitDepth = 0;
	this->fileLocation = const_cast<char*>("");
}