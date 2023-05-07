#include "Light.h"
Light::Light(){
	// This colour is the amount of light colour that means how much of R, G and B will be shown on the object
	this->colour = glm::vec3(1.0f, 1.0f, 1.0f);
	this->ambientIntensity = 1.0f;
	this->diffuseIntensity = 0.0f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat dIntensity){

	this->colour = glm::vec3(red, green, blue);
	this->ambientIntensity = aIntensity;
	this->diffuseIntensity = dIntensity;
}
Light::~Light(){
}