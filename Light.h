#pragma once
#include<glm/glm.hpp>
#include<GL/glew.h>
class Light
{
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat dIntensity);

	~Light();
protected:
	//Ambient Light
	glm::vec3 colour;
	GLfloat ambientIntensity; //Bright or Dim 
	
	//Diffuse Light
	//glm::vec3 direction;
	GLfloat diffuseIntensity;
};

