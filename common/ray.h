#ifndef RAY_H_
#define RAY_H_

#include <glm/glm.hpp>

#include <iostream>
#include <memory>

namespace es
{
	class Ray
	{
	public:
		Ray();
		Ray(const glm::vec3& o, const glm::vec3& d);
		~Ray();

		glm::vec3 getOrigin() const;

		glm::vec3 getDirection() const;

		glm::vec3 pointAtParameter(float t) const;

	private:
		glm::vec3 origin;
		glm::vec3 direction;
	};
}

#endif