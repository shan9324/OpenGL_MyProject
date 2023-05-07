#include "DirectionalLight.h"
DirectionalLight::DirectionalLight(): Light(){
	this->direction = glm::vec3(0.0f, -1.0f, 0.0f);
}
DirectionalLight::DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, 
									GLfloat aIntensity, GLfloat dIntensity,
									GLfloat xDir, GLfloat yDir, GLfloat zDir) :
							Light(red, green, blue, aIntensity, dIntensity) {

	this->direction = glm::vec3(xDir, yDir, zDir);
}
void DirectionalLight::UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation,
	GLfloat diffuseIntensityLocation, GLfloat directionLocation) {
	
	glUniform3f(ambientColourLocation, this->colour.x, this->colour.y, this->colour.z);
	glUniform1f(ambientIntensityLocation, this->ambientIntensity);
	glUniform3f(directionLocation, this->direction.x, this->direction.y, this->direction.z);
	glUniform1f(diffuseIntensityLocation, this->diffuseIntensity);
	
}