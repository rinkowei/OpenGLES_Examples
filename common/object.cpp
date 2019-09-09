#include "object.h"

namespace es
{
	Object::Object()
	{
		name = "None";

		position = glm::vec3(0.0f);
		rotation = glm::vec3(0.0f);
		scaling = glm::vec3(1.0f);
		model = glm::mat4(1.0f);

		autoUpdated = true;
		transformUpdated = true;
		isDirty = false;
	};

	Object::~Object()
	{

	}

	GLvoid Object::render(float deltaTime)
	{
		if (autoUpdated)
		{
			update(deltaTime);
		}
	}

	GLvoid Object::update(float deltaTime)
	{
		transformUpdated = false;
		if (isDirty)
		{
			model = glm::mat4(1.0f);

			model = glm::translate(model, position);

			model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

			model = glm::scale(model, scaling);

			isDirty = false;
		}
		transformUpdated = true;
	}

	GLvoid Object::setName(const std::string& name)
	{
		this->name = name;
	}
		
	GLvoid Object::translate(const glm::vec3& deltaPosition)
	{
		this->position += deltaPosition;
		isDirty = true;
	}

	GLvoid Object::rotate(const glm::vec3& deltaEuler)
	{
		this->rotation += deltaEuler;
		isDirty = true;
	}

	GLvoid Object::scale(const glm::vec3& deltaScale)
	{
		this->scaling += deltaScale;
		isDirty = true;
	}

	GLvoid Object::setPosition(const glm::vec3& position)
	{
		this->position = position;
		isDirty = true;
	}

	GLvoid Object::setRotation(const glm::vec3& euler)
	{
		this->rotation = euler;
		isDirty = true;
	}

	GLvoid Object::setScale(const glm::vec3& scale)
	{
		this->scaling = scale;
		isDirty = true;
	}

	const std::string& Object::getName() const
	{
		return name;
	}

	const glm::vec3& Object::getPosition() const
	{
		return position;
	}

	const glm::vec3& Object::getRotation() const
	{
		return rotation;
	}

	const glm::vec3& Object::getScaling() const
	{
		return scaling;
	}

	GLvoid Object::setModelMatrix(const glm::mat4& model)
	{
		this->model = model;
	}

	const glm::mat4& Object::getModelMatrix() const
	{
		return model;
	}

	GLvoid Object::setAutoUpdated(GLboolean autoUpdated)
	{
		this->autoUpdated = autoUpdated;
	}
}