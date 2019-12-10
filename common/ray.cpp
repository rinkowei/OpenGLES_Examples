#include "ray.h"

namespace es
{
	Ray::Ray()
	{

	}

	Ray::Ray(const glm::vec3& o, const glm::vec3& d)
	{
		origin = o;
		direction = d;
	}

	Ray::~Ray()
	{

	}

	glm::vec3 Ray::getOrigin() const
	{
		return origin;
	}

	glm::vec3 Ray::getDirection() const
	{
		return direction;
	}

	glm::vec3 Ray::pointAtParameter(float t) const
	{
		return origin + t * direction;
	}
}
