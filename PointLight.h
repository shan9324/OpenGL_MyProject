#pragma once
#include "Light.h"
class PointLight :public Light
{
public:
	PointLight();
	~PointLight();
	PointLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat dIntensity,
		GLfloat xPos, GLfloat yPos, GLfloat zPos,
		GLfloat cons, GLfloat lin, GLfloat exp);
	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation,
		GLfloat diffuseIntensityLocation, GLfloat positionLocation,
		GLfloat constantLocation, GLfloat linearLocation, GLfloat exponentLocation);

protected:

	glm::vec3 position;
	GLfloat constant;
	GLfloat linear;
	GLfloat exponent;
};

