#pragma once

#include <iostream>
#include <array>
#include "object.h"
#include "frustum.h"
#include "world.h"

namespace es
{
	class Camera : public Object
	{
	public:
		enum class Type
		{
			Perspective = 1,
			Orthographic = 2
		};

		static Camera* createPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);

		static Camera* createOrthographic(float zoomX, float zoomY, float nearPlane, float farPlane);

		static Camera* create();

		static Camera* getDefaultCamera();

		Camera::Type getType() const
		{
			return type;
		}

		void lookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));

		const glm::mat4& getProjectionMatrix() const;

		const glm::mat4& getViewMatrix() const;

		const glm::mat4& getViewProjectionMatrix() const;

		void setDepth(int8_t depth);

		int8_t getDepth() const
		{
			return depth;
		}

		float getFarPlane() const
		{
			return farPlane;
		}

		float getNearPlane() const
		{
			return nearPlane;
		}

		virtual void update(float deltaTime) override;

		virtual void translate(const glm::vec3& deltaPosition) override;

		virtual void rotate(const glm::vec3& deltaEuler) override;
	public:
		Camera();
		~Camera();

		bool initDefault();
		bool initPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
		bool initOrthoGraphic(float zoomX, float zoomY, float nearPlane, float farPlane);
	public:
		float rotationSpeed = 1.0f;
		float movementSpeed = 1.0f;
		float zoomSpeed = 1.0f;

		struct
		{
			bool left = false;
			bool right = false;
			bool up = false;
			bool down = false;
		} keys;
	protected:
		Camera::Type type;

		glm::mat4 projection;
		mutable glm::mat4 view;
		mutable glm::mat4 viewInv;
		mutable glm::mat4 viewProjection;

		glm::vec3 up;

		std::array<float, 2> zoom;
		float fov;
		float aspectRatio;
		float nearPlane;
		float farPlane;
		mutable bool viewProjectionDirty = true;
		bool viewProjectionUpdated = false;

		mutable Frustum frustum;
		mutable bool frustumDirty = true;

		int8_t depth = -1;
	};
}