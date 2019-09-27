#pragma once

#include "ogles.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>

namespace es
{
	class Object
	{
	public:
		Object();

		virtual ~Object();

		virtual GLvoid render(float deltaTime);

		virtual GLvoid update(float deltaTime);

		virtual GLvoid setName(const std::string& name);
		
		virtual GLvoid translate(const glm::vec3& deltaPosition);

		virtual GLvoid rotate(const glm::vec3& deltaEuler);

		virtual GLvoid scale(const glm::vec3& deltaScale);

		virtual GLvoid setPosition(const glm::vec3& position);

		virtual GLvoid setRotation(const glm::vec3& euler);

		virtual GLvoid setScale(const glm::vec3& scale);

		const std::string& getName() const;

		const glm::vec3& getPosition() const;

		const glm::vec3& getRotation() const;

		const glm::vec3& getScaling() const;

		GLvoid setModelMatrix(const glm::mat4& model);

		const glm::mat4& getModelMatrix() const;

		GLvoid setAutoUpdated(GLboolean autoUpdated);
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