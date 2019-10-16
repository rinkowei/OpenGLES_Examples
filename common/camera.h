#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <geometry.h>
#include <iostream>
#include <memory>
#include <array>

namespace es
{
	class Camera
	{
	public:
		Camera(float fov, float near, float far, float aspectRatio, const glm::vec3& position, const glm::vec3& forward);
		~Camera();

		static std::unique_ptr<Camera> create(float fov, float near, float far, float aspectRatio, const glm::vec3& position, const glm::vec3& forward);

		void setTranslationDelta(glm::vec3 direction, float amount);
		void setRotationDelta(glm::vec3 angle);
		void setPosition(glm::vec3 position);

		void update();

		void updateProjection(float fov, float near, float far, float aspectRatio);

		bool aabbInsideFrustum(glm::vec3 maxV, glm::vec3 minV);

		bool aabbInsidePlane(Plane plane, glm::vec3 maxV, glm::vec3 minV);

		const glm::vec3& getForward() const;

		const glm::vec3& getRight() const;

		const glm::mat4& getView() const;

		const glm::mat4& getProjection() const;
	private:
		float mFov;
		float mNear;
		float mFar;
		float mAspectRatio;

		glm::vec3 mPosition;
		glm::vec3 mForward;
		glm::vec3 mUp;
		glm::vec3 mRight;
		glm::vec3 mWorldUp;
		glm::quat mOrientation;

		float mYaw;
		float mPitch;
		float mRoll;

		glm::mat4 mModel;
		glm::mat4 mView;
		glm::mat4 mProjection;
		glm::mat4 mViewProjection;
		glm::mat4 mPrevViewProjection;
		glm::mat4 mRotate;
		glm::mat4 mTranslate;

		std::array<Plane, 6> mPlanes;

		Frustum mFrustum;
	};
}

#endif