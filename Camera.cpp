#include "Camera.h"

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch,
	GLfloat startMoveSpeed, GLfloat startTurnSpeed) :
	position(startPosition),
	front(glm::vec3(0.0f, 0.0f, -1.0f)),
	up(),
	right(),
	worldUp(startUp),
	yaw(startYaw),
	pitch(startPitch),
	movementSpeed(startMoveSpeed),
	turnSpeed(startTurnSpeed)
{
	this->update();
}

void Camera::update() {
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	
	front = glm::normalize(front);
	right = glm::normalize(glm::cross(front, worldUp) );
	up = glm::normalize(glm::cross(right, front));
}

glm::mat4 Camera::calculateViewMatrix() {
	return glm::lookAt(position, position + front, up);
}

void Camera::keyControl(bool* keys, GLfloat deltaTime){
	GLfloat velocity = movementSpeed * deltaTime;
	
	if (keys[GLFW_KEY_W]) {
		position += front * velocity;
	}
	if (keys[GLFW_KEY_S]) {
		position -= front * velocity;
	}
	if (keys[GLFW_KEY_A]) {
		position -= front * velocity;
	}
	if (keys[GLFW_KEY_D]) {
		position += front * velocity;
	}
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange) {
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch >= 89.0f) {
		pitch = 89.0f;
	}

	if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	update();
}

Camera::~Camera() {

}