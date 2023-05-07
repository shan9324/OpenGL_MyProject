#include "SpotLight.h"

SpotLight::SpotLight() : PointLight(), direction(glm::vec3(0.0f, 0.0f, 0.0f)), edge(0.0f), procEdge(cos(glm::radians(edge)))
{
}

SpotLight::~SpotLight()
{
}

SpotLight::SpotLight(GLfloat red, GLfloat green, GLfloat blue, 
	GLfloat aIntensity, GLfloat dIntensity, 
	GLfloat xPos, GLfloat yPos, GLfloat zPos, 
	GLfloat xDir, GLfloat yDir, GLfloat zDir, 
	GLfloat cons, GLfloat lin, GLfloat exp, 
	GLfloat edg) : PointLight(red, green, blue, aIntensity, dIntensity,xPos, yPos, zPos, cons, lin, exp),
				   direction(glm::vec3(xDir, yDir, zDir)),
				   edge(edg),
				   procEdge(cos(glm::radians(edge)))
	
{
}

void SpotLight::UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation, 
	GLfloat diffuseIntensityLocation, 
	GLfloat positionLocation, GLfloat directionLocation, 
	GLfloat constantLocation, GLfloat linearLocation, GLfloat exponentLocation, 
	GLfloat edgeLocation)
{

	glUniform3f(ambientColourLocation, this->colour.x, this->colour.y, this->colour.z);
	glUniform1f(ambientIntensityLocation, this->ambientIntensity);
	glUniform3f(positionLocation, this->position.x, this->position.y, this->position.z);
	glUniform3f(directionLocation, this->direction.x, this->direction.y, this->direction.z);
	glUniform1f(diffuseIntensityLocation, this->diffuseIntensity);
	glUniform1f(constantLocation, this->constant);
	glUniform1f(linearLocation, this->linear);
	glUniform1f(exponentLocation, this->exponent);
	glUniform1f(edgeLocation, this->procEdge);
}

void SpotLight::SetFlash(glm::vec3 pos, glm::vec3 dir)
{
	this->position = pos;
	this->direction = dir;
}

