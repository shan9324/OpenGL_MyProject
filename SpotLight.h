#pragma once
#include "PointLight.h"
class SpotLight : public PointLight
{
public:
	SpotLight();

	~SpotLight();
	SpotLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat dIntensity,
		GLfloat xPos, GLfloat yPos, GLfloat zPos,
		GLfloat xDir, GLfloat yDir, GLfloat zDir,
		GLfloat cons, GLfloat lin, GLfloat exp,
		GLfloat edg);
	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation,
		GLfloat diffuseIntensityLocation, GLfloat positionLocation, GLfloat directionLocation,
		GLfloat constantLocation, GLfloat linearLocation, GLfloat exponentLocation,
		GLfloat edgeLocation);

	void SetFlash(glm::vec3 pos, glm::vec3 dir);

private:
	glm::vec3 direction;
	GLfloat edge;
	GLfloat procEdge;

};

