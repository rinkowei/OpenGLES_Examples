#include "camera.h"

namespace es
{
	Camera::Camera(float fov, float near, float far, float aspectRatio, const glm::vec3& position, const glm::vec3& forward)
		 :mMoveSensitivity(10.0f),
		 mPosition(position),
		 mIsDirty(true)
	{
		mFront = glm::normalize(forward);
		mWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		mRight = glm::normalize(glm::cross(mFront, mWorldUp));
		mUp = glm::normalize(glm::cross(mRight, mFront));

		mRoll = 0.0f;
		mPitch = 0.0f;
		mYaw = 0.0f;

		mOrientation = glm::quat();
		mProjection = glm::perspective(glm::radians(fov), aspectRatio, near, far);
		mPrevViewProjection = glm::mat4(1.0f);

		mFrustum.mFov = fov;
		mFrustum.mNear = near;
		mFrustum.mFar = far;
		mFrustum.mAspectRatio = aspectRatio;
	
		update();
	}

	Camera::~Camera()
	{

	}

	std::unique_ptr<Camera> Camera::create(float fov, float near, float far, float aspectRatio, const glm::vec3& position, const glm::vec3& forward)
	{
		return std::make_unique<Camera>(fov, near, far, aspectRatio, position, forward);
	}

	void Camera::setTranslationDelta(glm::vec3 direction, float rate)
	{
		mPosition += direction * rate * mMoveSensitivity;
		mIsDirty = true;
	}

	void Camera::setRotationDelta(glm::vec3 angle)
	{
		mYaw += glm::radians(angle.y);
		mPitch += glm::radians(angle.x);
		mRoll += glm::radians(angle.z);
		mIsDirty = true;
	}

	void Camera::setRotation(glm::vec3 rotation)
	{
		mYaw = glm::radians(rotation.y);
		mPitch = glm::radians(rotation.x);
		mRoll = glm::radians(rotation.z);
		mIsDirty = true;
	}

	void Camera::setPosition(glm::vec3 position)
	{
		mPosition = position;
		mIsDirty = true;
	}

	void Camera::update()
	{
		if (mIsDirty)
		{
			glm::quat qPitch = glm::angleAxis(mPitch, glm::vec3(1.0f, 0.0f, 0.0f));
			glm::quat qYaw = glm::angleAxis(mYaw, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::quat qRoll = glm::angleAxis(mRoll, glm::vec3(0.0f, 0.0f, 1.0f));

			mOrientation = qPitch * mOrientation;
			mOrientation = mOrientation * qYaw;
			mOrientation = qRoll * mOrientation;
			mOrientation = glm::normalize(mOrientation);

			mRotate = glm::mat4_cast(mOrientation);
			mFront = glm::conjugate(mOrientation) * glm::vec3(0.0f, 0.0f, -1.0f);

			mRight = glm::normalize(glm::cross(mFront, mWorldUp));
			mUp = glm::normalize(glm::cross(mRight, mFront));

			mTranslate = glm::translate(glm::mat4(1.0f), -mPosition);
			mView = mRotate * mTranslate;
			mPrevViewProjection = mViewProjection;
			mViewProjection = mProjection * mView;

			updateFrustum();

			mIsDirty = false;
		}
	}

	void Camera::updateProjection(float fov, float near, float far, float aspectRatio)
	{
		mProjection = glm::perspective(glm::radians(fov), aspectRatio, near, far);
	}

	void Camera::updateFrustum()
	{
		float nearHeight = 2 * tan(mFrustum.mFov / 2) * mFrustum.mNear;
		float nearWidth = nearHeight * mFrustum.mAspectRatio;

		float farHeight = 2 * tan(mFrustum.mFov / 2) * mFrustum.mFar;
		float farWidth = farHeight * mFrustum.mAspectRatio;

		glm::vec3 fc = mPosition + mFront * mFrustum.mFar;
		glm::vec3 nc = mPosition + mFront * mFrustum.mNear;

		mFrustum.mFrustumCorners[4] = fc + (mUp * farHeight / 2.0f) - (mRight * farWidth / 2.0f); // 
		mFrustum.mFrustumCorners[5] = fc + (mUp * farHeight / 2.0f) + (mRight * farWidth / 2.0f); // 
		mFrustum.mFrustumCorners[7] = fc - (mUp * farHeight / 2.0f) - (mRight * farWidth / 2.0f); // 
		mFrustum.mFrustumCorners[6] = fc - (mUp * farHeight / 2.0f) + (mRight * farWidth / 2.0f); // 

		mFrustum.mFrustumCorners[0] = nc + (mUp * nearHeight / 2.0f) - (mRight * nearWidth / 2.0f); // 
		mFrustum.mFrustumCorners[1] = nc + (mUp * nearHeight / 2.0f) + (mRight * nearWidth / 2.0f); // 
		mFrustum.mFrustumCorners[3] = nc - (mUp * nearHeight / 2.0f) - (mRight * nearWidth / 2.0f); // 
		mFrustum.mFrustumCorners[2] = nc - (mUp * nearHeight / 2.0f) + (mRight * nearWidth / 2.0f); // 
	}

	const glm::vec3& Camera::getPosition() const
	{
		return mPosition;
	}


	const glm::vec3& Camera::getForward() const
	{
		return mFront;
	}

	const glm::vec3& Camera::getRight() const
	{
		return mRight;
	}

	const glm::mat4& Camera::getView() const
	{
		return mView;
	}

	const glm::mat4& Camera::getProjection() const
	{
		return mProjection;
	}

	void Camera::setMoveSensitivity(float sensitivity)
	{
		mMoveSensitivity = sensitivity;
	}

	float Camera::getMoveSensitivity() const
	{
		return mMoveSensitivity;
	}

	float Camera::getFov() const
	{
		return mFrustum.mFov;
	}

	float Camera::getNearPlane() const
	{
		return mFrustum.mNear;
	}

	float Camera::getFarPlane() const
	{
		return mFrustum.mFar;
	}

	float Camera::getAspectRatio() const
	{
		return mFrustum.mAspectRatio;
	}

	const Frustum& Camera::getFrustum() const
	{
		return mFrustum;
	}
}