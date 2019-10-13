#include "object.h"

namespace es
{
	Object::Object(const std::string& name)
		:mName(name),
		 mPosition(glm::vec3(0.0f)),
		 mRotation(glm::vec3(0.0f)),
		 mScaling(glm::vec3(1.0f)),
		 mModelMatrix(glm::mat4(1.0f)),
		 mAutoUpdated(true),
		 mTransformUpdated(true),
		 mIsDirty(false)
	{

	};

	Object::~Object()
	{

	}

	std::shared_ptr<Object> Object::create(const std::string& name)
	{
		return std::make_shared<Object>(name);
	}

	GLvoid Object::render(float deltaTime)
	{
		if (mAutoUpdated)
		{
			update(deltaTime);
		}
	}

	GLvoid Object::update(float deltaTime)
	{
		mTransformUpdated = false;
		if (mIsDirty)
		{
			mModelMatrix = glm::mat4(1.0f);

			mModelMatrix = glm::translate(mModelMatrix, mPosition);

			mModelMatrix = glm::rotate(mModelMatrix, glm::radians(mRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			mModelMatrix = glm::rotate(mModelMatrix, glm::radians(mRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			mModelMatrix = glm::rotate(mModelMatrix, glm::radians(mRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

			mModelMatrix = glm::scale(mModelMatrix, mScaling);

			mIsDirty = false;
		}
		mTransformUpdated = true;
	}

	void Object::setName(const std::string& name)
	{
		this->mName = name;
	}
		
	void Object::translate(const glm::vec3& deltaPosition)
	{
		this->mPosition += deltaPosition;
		mIsDirty = true;
	}

	void Object::rotate(const glm::vec3& deltaEuler)
	{
		this->mRotation += deltaEuler;
		mIsDirty = true;
	}

	void Object::scale(const glm::vec3& deltaScale)
	{
		this->mScaling += deltaScale;
		mIsDirty = true;
	}

	void Object::setPosition(const glm::vec3& position)
	{
		this->mPosition = position;
		mIsDirty = true;
	}

	void Object::setRotation(const glm::vec3& euler)
	{
		this->mRotation = euler;
		mIsDirty = true;
	}

	void Object::setScale(const glm::vec3& scale)
	{
		this->mScaling = scale;
		mIsDirty = true;
	}

	const std::string& Object::getName() const
	{
		return mName;
	}

	const glm::vec3& Object::getPosition() const
	{
		return mPosition;
	}

	const glm::vec3& Object::getRotation() const
	{
		return mRotation;
	}

	const glm::vec3& Object::getScaling() const
	{
		return mScaling;
	}

	void Object::setModelMatrix(const glm::mat4& model)
	{
		this->mModelMatrix = model;
	}

	const glm::mat4& Object::getModelMatrix() const
	{
		return mModelMatrix;
	}

	void Object::setAutoUpdated(bool autoUpdated)
	{
		this->mAutoUpdated = autoUpdated;
	}
}