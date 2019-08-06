#pragma once

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
			this->position = glm::vec3(0.0f);
			this->rotation = glm::vec3(0.0f);
			this->scaling = glm::vec3(1.0f);
			this->model = glm::mat4(1.0f);
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
			model = glm::translate(model, deltaPosition);
		}

		void rotate(const glm::vec3& deltaRotation)
		{
			glm::qua<float> q = glm::qua<float>(glm::radians(deltaRotation));
			model = glm::mat4_cast(q) * model;
		}

		void scale(const glm::vec3& deltaScale)
		{
			model = glm::scale(model, deltaScale);
		}
	protected:
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scaling;

		glm::mat4 model;
	private:

	};
}