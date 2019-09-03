#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace es
{
	class Object
	{
	public:
		Object()
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

		virtual ~Object() = default;

		virtual void render(float deltaTime)
		{
			if (autoUpdated)
			{
				update(deltaTime);
			}
		}

		virtual void update(float deltaTime)
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

		virtual void setName(const std::string& name)
		{
			this->name = name;
		}
		
		virtual void translate(const glm::vec3& deltaPosition)
		{
			this->position += deltaPosition;
			isDirty = true;
		}

		virtual void rotate(const glm::vec3& deltaEuler)
		{
			this->rotation += deltaEuler;
			isDirty = true;
		}

		virtual void scale(const glm::vec3& deltaScale)
		{
			this->scaling += deltaScale;
			isDirty = true;
		}

		virtual void setPosition(const glm::vec3& position)
		{
			this->position = position;
			isDirty = true;
		}

		virtual void setRotation(const glm::vec3& euler)
		{
			this->rotation = euler;
			isDirty = true;
		}

		virtual void setScale(const glm::vec3& scale)
		{
			this->scaling = scale;
			isDirty = true;
		}

		const std::string& getName() const
		{
			return name;
		}

		const glm::vec3& getPosition() const
		{
			return position;
		}

		const glm::vec3& getRotation() const
		{
			return rotation;
		}

		const glm::vec3& getScaling() const
		{
			return scaling;
		}

		void setModelMatrix(const glm::mat4& model)
		{
			this->model = model;
		}

		const glm::mat4& getModelMatrix() const
		{
			return model;
		}

		void setAutoUpdated(GLboolean autoUpdated)
		{
			this->autoUpdated = autoUpdated;
		}
	protected:
		std::string name;

		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scaling;
		glm::mat4 model;

		// is auto call update function in render()
		GLboolean autoUpdated;
		// is transformation updated
		GLboolean transformUpdated;
		// is transform updated not yet
		GLboolean isDirty;
	};
}