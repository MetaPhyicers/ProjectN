#pragma once
#include "Utility.h"


class Camera
{
public:
	Camera();
	virtual ~Camera();

	void InitSpeed(float velocity, float mouse_sensitive);

	void GoUp();
	void GoDown();
	void GoLeft();
	void GoRight();
	void GoForward();
	void GoBack(); 

	void YawAndPitch(float yawOffset, float pitchOffset);
	void SetLookAt(glm::vec3& target);
	void SetPos(glm::vec3& pos);

	glm::mat4& GetViewMatrix(float dt);
	glm::vec3 GetCameraPos();
	glm::vec3 GetCameraFront();
	glm::vec3 GetCameraUp();

private:

	float mouseSpeed;
	float wanderSpeed;

	glm::vec3 cameraPos;
	glm::vec3 cameraUp;
	glm::vec3 cameraTarget;

	glm::mat4 view;

	float pitch;
	float yaw;
};

