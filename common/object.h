#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "world.h"

using namespace es;

namespace es
{
	class Object
	{
	public:
		Object()
		{
			position = glm::vec3(0.0f);
			rotation = glm::vec3(0.0f);
			scaling = glm::vec3(1.0f);
			model = glm::mat4(1.0f);

			camera = World::getWorld()->getMainCamera();

			transformUpdated = true;
			isDirty = false;
		};

		virtual ~Object() = default;

		virtual void render()
		{

		}

		virtual void update()
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
		
		void translate(const glm::vec3& deltaPosition)
		{
			this->position += deltaPosition;
			isDirty = true;
		}

		void rotate(const glm::vec3& deltaEuler)
		{
			this->rotation += deltaEuler;
			isDirty = true;
		}

		void scale(const glm::vec3& deltaScale)
		{
			this->scaling += deltaScale;
			isDirty = true;
		}

		void setPosition(const glm::vec3& position)
		{
			this->position = position;
			isDirty = true;
		}

		void setRotation(const glm::vec3& euler)
		{
			this->rotation = euler;
			isDirty = true;
		}

		void setScale(const glm::vec3& scale)
		{
			this->scaling = scale;
			isDirty = true;
		}

	protected:
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scaling;

		glm::mat4 model;

		Camera* camera;

		GLboolean transformUpdated;
	private:
		GLboolean isDirty;
	};
}