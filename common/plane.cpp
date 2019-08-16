#include "plane.h"

namespace es
{
	Plane::Plane()
		:normal(0.0f, 0.0f, 1.0f),
		 dist(0.0f)
	{

	}

	Plane::Plane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
	{
		initPlane(p1, p2, p3);
	}

	Plane::Plane(const glm::vec3& normal, GLfloat dist)
	{
		initPlane(normal, dist);
	}

	Plane::Plane(const glm::vec3& normal, const glm::vec3& point)
	{
		initPlane(normal, point);
	}

	void Plane::initPlane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
	{
		glm::vec3 p21 = p2 - p1;
		glm::vec3 p32 = p3 - p2;
		normal = glm::normalize(glm::cross(p21, p32));
		dist = glm::dot(normal, p1);
	}

	void Plane::initPlane(const glm::vec3& normal, GLfloat dist)
	{
		GLfloat oneOverLength = 1.0f / glm::length(normal);
		this->normal = normal * oneOverLength;
		this->dist = dist * oneOverLength;
	}

	void Plane::initPlane(const glm::vec3& normal, const glm::vec3& point)
	{
		this->normal = glm::normalize(normal);
		this->dist = glm::dot(this->normal, point);
	}

	GLfloat Plane::dist2Plane(const glm::vec3& p) const
	{
		return glm::dot(normal, p) - dist;
	}

	PointSide Plane::getSide(const glm::vec3& point) const
	{
		GLfloat dist = dist2Plane(point);
		if (dist > 0)
			return PointSide::FrontPlane;
		else if (dist < 0)
			return PointSide::BehindPlane;
		else
			return PointSide::InPlane;
	}
}