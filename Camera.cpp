#include "Camera.h"


Camera::Camera()
	: cameraPos(0.f, 0.f, 3.f), cameraUp(0.f, 1.0f, 0.f),
	cameraTarget(0.f, 0.f, -1.f), view(1.f), yaw(90.f), pitch(0.f)
{
	view = glm::lookAt(cameraPos, cameraPos + cameraTarget, cameraUp);
}


Camera::~Camera()
{
}

void Camera::InitSpeed(float velocity, float mouseVelocity)
{
	wanderSpeed = velocity;
	mouseSpeed = mouseVelocity;
}

void Camera::GoUp()
{
	cameraPos += wanderSpeed * cameraUp;
}

void Camera::GoDown()
{
	cameraPos -= wanderSpeed * cameraUp;
}

void Camera::GoLeft()
{
	// in right-hand coordinate, the result of cross product should be nagetive
	glm::vec3 normal = -glm::cross(cameraTarget, cameraUp);
	normal = glm::normalize(normal);
	cameraPos += wanderSpeed * normal;
}

void Camera::GoRight()
{
	// in right-hand coordinate, the result of cross product should be nagetive
	glm::vec3 normal = -glm::cross(cameraTarget, cameraUp);
	normal = glm::normalize(normal);
	cameraPos -= wanderSpeed * normal;
}

void Camera::GoForward()
{
	cameraPos += wanderSpeed * cameraTarget;
}

void Camera::GoBack()
{
	cameraPos -= wanderSpeed * cameraTarget;
}

void Camera::YawAndPitch(float yawOffset, float pitchOffset)
{
	yaw += yawOffset * mouseSpeed;
	pitch += pitchOffset * mouseSpeed;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;


	glm::vec3 front;
	front.x = -cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = -sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	cameraTarget = glm::normalize(front);
	glm::vec3 right = glm::normalize(glm::cross(cameraTarget, cameraUp));
	cameraUp = glm::normalize(glm::cross(right, cameraTarget));
}

void Camera::SetLookAt(glm::vec3& target)
{
	cameraTarget = target - cameraPos;
}

void Camera::SetPos(glm::vec3& pos)
{
	cameraPos = pos;
}

glm::mat4& Camera::GetViewMatrix(float dt)
{
	view = glm::lookAt(cameraPos, cameraPos + cameraTarget, cameraUp);

	wanderSpeed = dt * 95.f;
	
	return view;
}

glm::vec3 Camera::GetCameraPos()
{
	return cameraPos;
}

glm::vec3 Camera::GetCameraFront()
{
	return cameraTarget;
}

glm::vec3 Camera::GetCameraUp()
{
	return cameraUp;
}

