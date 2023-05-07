#include "PointLight.h"

PointLight::PointLight() : Light()
{
	this->position = glm::vec3(0.0f, 0.0f, 0.0f);
	constant = 1.0f;
	linear = 0.0f;
	exponent = 0.0f;
}

PointLight::PointLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat dIntensity, 
						GLfloat xPos, GLfloat yPos, GLfloat zPos,
						GLfloat cons, GLfloat lin, GLfloat exp) : Light(red,green,blue,aIntensity, dIntensity)
{
	this->position = glm::vec3(xPos, yPos, zPos);
	constant = cons;
	linear = lin;
	exponent = exp;
}

void PointLight::UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation, GLfloat diffuseIntensityLocation, GLfloat positionLocation,
	GLfloat constantLocation, GLfloat linearLocation, GLfloat exponentLocation)
{
	glUniform3f(ambientColourLocation, this->colour.x, this->colour.y, this->colour.z);
	glUniform1f(ambientIntensityLocation, this->ambientIntensity);
	glUniform3f(positionLocation, this->position.x, this->position.y, this->position.z);
	glUniform1f(diffuseIntensityLocation, this->diffuseIntensity);
	glUniform1f(constantLocation, this->constant);
	glUniform1f(linearLocation, this->linear);
	glUniform1f(exponentLocation, this->exponent);
}

PointLight::~PointLight()
{
}