#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
const float MAX_FOV = 45.0f;
const float MIN_FOV = 1.0f;

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

	Camera(float aspect, float znear, float zfar, glm::vec3 position = glm::vec3(0.0f, 0.0f, 1.0f))
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
		setViewMatrix();
	}

	void handleMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true)
	{
		xOffset *= mouseSensitivity;
		yOffset *= mouseSensitivity;

		yaw += xOffset;
		pitch += yOffset;

		if (constrainPitch)
		{
			if (pitch > 89.0f)
				pitch = 89.0f;
			else if (pitch < -89.0f)
				pitch = -89.0f;
		}

		updateCameraProperties();
	}

	void handleMouseScroll(float yOffset)
	{
		if (fov >= MIN_FOV && fov <= MAX_FOV)
			fov -= yOffset;
		if (fov <= MIN_FOV)
			fov = MIN_FOV;
		if (fov >= MAX_FOV)
			fov = MAX_FOV;

		setPerspectiveMatrix();
	}

	glm::vec3 getPosition()
	{
		return position;
	}

	glm::vec3 getFront()
	{
		return front;
	}

private:
	// camera's properties
	float fov = 45.0f;
	float aspectRatio;
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
	float znear;
	float zfar;

	// euler angle rotate around Y-axis
	float yaw = -90.0f;
	// euler angle rotate around X-axis
	float pitch = 0.0f;

	// camera operate options
	float movementSpeed = 2.5f;
	float mouseSensitivity = 0.1f;

	// matrices
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	glm::mat4 perspectiveMatrix = glm::mat4(1.0f);

	// recalculate the front vector from the camera's updated euler angles
	void updateCameraProperties()
	{
		glm::vec3 updatedFront;
		updatedFront.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
		updatedFront.y = glm::sin(glm::radians(pitch));
		updatedFront.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
		this->front = glm::normalize(updatedFront);

		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));

		setViewMatrix();
	}
};