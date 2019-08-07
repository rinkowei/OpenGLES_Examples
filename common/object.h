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
			this->position = glm::vec3(0.0f);
			this->rotation = glm::vec3(0.0f);
			this->scaling = glm::vec3(1.0f);
			this->model = glm::mat4(1.0f);

			camera = World::getWorld()->getMainCamera();
		};

		virtual ~Object() = default;

		virtual void render()
		{

		}

		virtual void update()
		{

		}
		
		void translate(const glm::vec3& deltaPosition)
		{
			this->position += deltaPosition;
			model = glm::translate(model, deltaPosition);
		}

		void rotate(const glm::vec3& deltaRotation)
		{
			this->rotation += deltaRotation;
			//glm::qua<float> q = glm::qua<float>(deltaRotation);
			//model = glm::mat4_cast(q) * model;
	
			model = glm::rotate(model, glm::radians(deltaRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(deltaRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(deltaRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			
		}

		void scale(const glm::vec3& deltaScale)
		{
			this->scaling += deltaScale;
			model = glm::scale(model, deltaScale);
		}

		void setPosition(const glm::vec3& position)
		{
			model = glm::translate(model, -this->position);
			model = glm::translate(model, position);
			this->position = position;
		}

		void setRotation(const glm::vec3& rotation)
		{
			model = glm::rotate(model, glm::radians(-this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

			model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			
			/*
			model = glm::mat4(1.0f);
			model = glm::scale(model, this->scaling);
			model = glm::translate(model, this->position);
			model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			*/
			this->rotation = rotation;
		}

		void setScale(const glm::vec3& scale)
		{
			model = glm::scale(model, 1.0f / this->scaling);
			model = glm::scale(model, scale);
			this->scaling = scale;
		}

	protected:
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scaling;

		glm::mat4 model;

		Camera* camera;
	};
}