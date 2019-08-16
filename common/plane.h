#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <iostream>

namespace es
{
	enum class PointSide
	{
		InPlane,
		FrontPlane,
		BehindPlane
	};

	class Plane
	{
	public:
		// create a default plane whose normal is (0, 0, 1), and dist is 0, xoy plane in fact
		Plane();

		// create plane from three point
		Plane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

		// create plane from normal and dist
		Plane(const glm::vec3& normal, GLfloat dist);

		// create plane from normal and a point on plane
		Plane(const glm::vec3& normal, const glm::vec3& point);

		// init plane from three point
		void initPlane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

		// init plane from normal and dist
		void initPlane(const glm::vec3& normal, GLfloat dist);

		// init plane from normal and a point on plane
		void initPlane(const glm::vec3& normal, const glm::vec3& point);

		// dist to plane, > 0 normal direction
		GLfloat dist2Plane(const glm::vec3& p) const;

		// get the plane's normal
		const glm::vec3& getNormal() const
		{
			return normal;
		}
		
		// get the plane's distance to the origin along its normal
		GLfloat getDist() const
		{
			return dist;
		}

		// return the side where the point is
		PointSide getSide(const glm::vec3& point) const;
	protected:
		// the normal line of the plane
		glm::vec3 normal;

		// original displacement of the normal
		GLfloat dist;
	};
}