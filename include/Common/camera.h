#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	enum class CameraMovement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	Camera(float aspect, float znear, float zfar, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f))
		:front(glm::vec3(0.0f, 0.0f, -1.0f)),
		 movementSpeed(2.5f),
		 mouseSensitivity(0.1f),
		 fov(45.0f)
	{
		this->position = position;
		this->aspectRatio = aspect;
		this->znear = znear;
		this->zfar = zfar;
		setViewMatrix();
		setPerspectiveMatrix();
	}
	
	void setViewMatrix()
	{
		viewMatrix = glm::lookAt(position, position + front, up);
	}

	glm::mat4 getViewMatrix()
	{
		return viewMatrix;
	}

	void setPerspectiveMatrix()
	{
		perspectiveMatrix = glm::perspective(glm::radians(fov), aspectRatio, znear, zfar);
	}

	glm::mat4 getPerspectiveMatrix()
	{
		return perspectiveMatrix;
	}

	void handleKeyboard(CameraMovement direction, float deltaTime)
	{
		float velocity = movementSpeed * deltaTime;
		if (direction == CameraMovement::FORWARD)
		{
			position += front * velocity;
		}
		else if (direction == CameraMovement::BACKWARD)
		{
			position -= front * velocity;
		}
		else if (direction == CameraMovement::LEFT)
		{
			position -= right * velocity;
		}
		else if (direction == CameraMovement::RIGHT)
		{
			position += right * velocity;
		}
	}

	void handleMouseScroll(float yoffset)
	{
		
	}

private:
	float fov = 45.0f;
	float movementSpeed = 2.5;
	float mouseSensitivity = 0.1;

	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	float yaw;
	float pitch;

	float znear;
	float zfar;

	float aspectRatio;

	glm::mat4 viewMatrix = glm::mat4(1.0f);
	glm::mat4 perspectiveMatrix = glm::mat4(1.0f);
};