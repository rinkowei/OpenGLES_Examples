#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace es
{
	class Camera
	{
	private:
		float fov;
		float aspect;
		float znear;
		float zfar;

		void updateViewMatrix()
		{
			glm::mat4 rotMatrix = glm::mat4(1.0f);
			glm::mat4 transMatrix;

			rotMatrix = glm::rotate(rotMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			rotMatrix = glm::rotate(rotMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			rotMatrix = glm::rotate(rotMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

			transMatrix = glm::translate(glm::mat4(1.0f), position);
			
			if (type == Camera::Type::firstPerson)
			{
				matrices.view = rotMatrix * transMatrix;
			}
			else
			{
				matrices.view = transMatrix * rotMatrix;
			}
		
			updated = true;
		};
	public:
		enum class Type
		{
			lookAt,
			firstPerson
		};

		Camera()
		{
			this->type = Camera::Type::firstPerson;
			setPerspective(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
			updateViewMatrix();
		}

		Camera(float fov, float aspect, float znear, float zfar, Camera::Type type)
		{
			this->type = type;
			setPerspective(fov, aspect, znear, zfar);
			updateViewMatrix();
		}

		~Camera() = default;

		Camera::Type type = Camera::Type::firstPerson;

		glm::vec3 rotation = glm::vec3(0.0f);
		glm::vec3 position = glm::vec3(0.0f);

		float rotationSpeed = 1.0f;
		float movementSpeed = 1.0f;
		float zoomSpeed = 1.0f;

		bool updated = false;

		struct
		{
			glm::mat4 projection;
			glm::mat4 view;
		} matrices;

		struct
		{
			bool left = false;
			bool right = false;
			bool up = false;
			bool down = false;
		} keys;

		bool moving()
		{
			return keys.left || keys.right || keys.up || keys.down;
		}

		float getNearPlane()
		{
			return znear;
		}

		float getFarPlane()
		{
			return zfar;
		}

		void setPerspective(float fov, float aspect, float znear, float zfar)
		{
			this->fov = fov;
			this->aspect = aspect;
			this->znear = znear;
			this->zfar = zfar;
			matrices.projection = glm::perspective(glm::radians(fov), aspect, znear, zfar);
		}

		void updateAspectRatio(float aspect)
		{
			matrices.projection = glm::perspective(glm::radians(fov), aspect, znear, zfar);
		}

		void setPosition(glm::vec3 position)
		{
			this->position = position;
			updateViewMatrix();
		}

		void rotate(glm::vec3 delta)
		{
			this->rotation += delta;
			updateViewMatrix();
		}

		void translate(glm::vec3 delta)
		{
			this->position += delta;
			updateViewMatrix();
		}

		void update(float deltaTime)
		{
			updated = false;
			if (type == Camera::Type::firstPerson)
			{
				if (moving())
				{
					glm::vec3 camFront;
					camFront.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
					camFront.y = sin(glm::radians(rotation.x));
					camFront.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
					camFront = glm::normalize(camFront);

					float moveSpeed = deltaTime * movementSpeed;

					if (keys.up)
						position += camFront * moveSpeed;
					if (keys.down)
						position -= camFront * moveSpeed;
					if (keys.left)
						position -= glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeed;
					if (keys.right)
						position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeed;

					updateViewMatrix();
				}
			}
		};

		// Update camera passing separate axis data (gamepad)
	    // Returns true if view or position has been changed
		bool updatePad(glm::vec2 axisLeft, glm::vec2 axisRight, float deltaTime)
		{
			bool retVal = false;

			if (type == Camera::Type::firstPerson)
			{
				// Use the common console thumbstick layout		
				// Left = view, right = move

				const float deadZone = 0.0015f;
				const float range = 1.0f - deadZone;

				glm::vec3 camFront;
				camFront.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
				camFront.y = sin(glm::radians(rotation.x));
				camFront.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
				camFront = glm::normalize(camFront);

				float moveSpeed = deltaTime * movementSpeed * 2.0f;
				float rotSpeed = deltaTime * rotationSpeed * 50.0f;

				// Move
				if (fabsf(axisLeft.y) > deadZone)
				{
					float pos = (fabsf(axisLeft.y) - deadZone) / range;
					position -= camFront * pos * ((axisLeft.y < 0.0f) ? -1.0f : 1.0f) * moveSpeed;
					retVal = true;
				}
				if (fabsf(axisLeft.x) > deadZone)
				{
					float pos = (fabsf(axisLeft.x) - deadZone) / range;
					position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * pos * ((axisLeft.x < 0.0f) ? -1.0f : 1.0f) * moveSpeed;
					retVal = true;
				}

				// Rotate
				if (fabsf(axisRight.x) > deadZone)
				{
					float pos = (fabsf(axisRight.x) - deadZone) / range;
					rotation.y += pos * ((axisRight.x < 0.0f) ? -1.0f : 1.0f) * rotSpeed;
					retVal = true;
				}
				if (fabsf(axisRight.y) > deadZone)
				{
					float pos = (fabsf(axisRight.y) - deadZone) / range;
					rotation.x -= pos * ((axisRight.y < 0.0f) ? -1.0f : 1.0f) * rotSpeed;
					retVal = true;
				}
			}
			else
			{
				// todo: move code from example base class for look-at
			}

			if (retVal)
			{
				updateViewMatrix();
			}

			return retVal;
		}
	};
}