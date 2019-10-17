#ifndef OBJECT_H_
#define OBJECT_H_

#include <ogles.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <memory>

namespace es
{
	class Object
	{
	public:
		Object() = default;
		Object(const std::string& name);

		virtual ~Object();

		static std::shared_ptr<Object> create(const std::string& name);

		virtual void render();

		virtual void update();

		virtual void setName(const std::string& name);
		
		virtual void translate(const glm::vec3& deltaPosition);

		virtual void rotate(const glm::vec3& deltaEuler);

		virtual void scale(const glm::vec3& deltaScale);

		virtual void setPosition(const glm::vec3& position);

		virtual void setRotation(const glm::vec3& euler);

		virtual void setScale(const glm::vec3& scale);

		const std::string& getName() const;

		const glm::vec3& getPosition() const;

		const glm::vec3& getRotation() const;

		const glm::vec3& getScaling() const;

		void setModelMatrix(const glm::mat4& model);

		const glm::mat4& getModelMatrix() const;

		void setAutoUpdated(bool autoUpdated);
	protected:
		std::string mName;

		glm::vec3 mPosition;
		glm::vec3 mRotation;
		glm::vec3 mScaling;

		glm::mat4 mModelMatrix;

		// is auto call update function in render()
		bool mAutoUpdated;
		// is transformation updated
		bool mTransformUpdated;
		// is transform updated not yet
		bool mIsDirty;
	};
}

#endif