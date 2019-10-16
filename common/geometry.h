#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <glm/glm.hpp>

#include <iostream>
#include <memory>
#include <array>
#include <vector>

namespace es
{
	enum FrustumPlanes
	{
		FRUSTUM_PLANE_NEAR = 0,
		FRUSTUM_PLANE_FAR = 1,
		FRUSTUM_PLANE_LEFT = 2,
		FRUSTUM_PLANE_RIGHT = 3,
		FRUSTUM_PLANE_TOP = 4,
		FRUSTUM_PLANE_BOTTOM = 5
	};

	struct Plane
	{
		glm::vec3 n;
		float     d;
	};

	struct Frustum
	{
		std::array<Plane, 6> planes;
	};

	struct AABB
	{
		glm::vec3 min;
		glm::vec3 max;
	};

	inline void frustumFromMatrix(Frustum& frustum, const glm::mat4& viewProj)
	{
		frustum.planes[FRUSTUM_PLANE_RIGHT].n = glm::vec3(viewProj[0][3] - viewProj[0][0],
			viewProj[1][3] - viewProj[1][0],
			viewProj[2][3] - viewProj[2][0]);
		frustum.planes[FRUSTUM_PLANE_RIGHT].d = viewProj[3][3] - viewProj[3][0];

		frustum.planes[FRUSTUM_PLANE_LEFT].n = glm::vec3(viewProj[0][3] + viewProj[0][0],
			viewProj[1][3] + viewProj[1][0],
			viewProj[2][3] + viewProj[2][0]);
		frustum.planes[FRUSTUM_PLANE_LEFT].d = viewProj[3][3] + viewProj[3][0];

		frustum.planes[FRUSTUM_PLANE_BOTTOM].n = glm::vec3(viewProj[0][3] + viewProj[0][1],
			viewProj[1][3] + viewProj[1][1],
			viewProj[2][3] + viewProj[2][1]);
		frustum.planes[FRUSTUM_PLANE_BOTTOM].d = viewProj[3][3] + viewProj[3][1];

		frustum.planes[FRUSTUM_PLANE_TOP].n = glm::vec3(viewProj[0][3] - viewProj[0][1],
			viewProj[1][3] - viewProj[1][1],
			viewProj[2][3] - viewProj[2][1]);
		frustum.planes[FRUSTUM_PLANE_TOP].d = viewProj[3][3] - viewProj[3][1];

		frustum.planes[FRUSTUM_PLANE_FAR].n = glm::vec3(viewProj[0][3] - viewProj[0][2],
			viewProj[1][3] - viewProj[1][2],
			viewProj[2][3] - viewProj[2][2]);
		frustum.planes[FRUSTUM_PLANE_FAR].d = viewProj[3][3] - viewProj[3][2];

		frustum.planes[FRUSTUM_PLANE_NEAR].n = glm::vec3(viewProj[0][3] + viewProj[0][2],
			viewProj[1][3] + viewProj[1][2],
			viewProj[2][3] + viewProj[2][2]);
		frustum.planes[FRUSTUM_PLANE_NEAR].d = viewProj[3][3] + viewProj[3][2];

		// normalize them
		for (int i = 0; i < 6; i++)
		{
			float invl = sqrt(frustum.planes[i].n.x * frustum.planes[i].n.x + frustum.planes[i].n.y * frustum.planes[i].n.y + frustum.planes[i].n.z * frustum.planes[i].n.z);
			frustum.planes[i].n /= invl;
			frustum.planes[i].d /= invl;
		}
	}

	inline void extractFrustumCorners(const glm::mat4& invViewProj, std::vector<glm::vec3>& corners)
	{
		const std::array<glm::vec4, 8> kFrustumCorners =
		{
			glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f),
			glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f),
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),
			glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f),
			glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f),
			glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),
			glm::vec4(1.0f, -1.0f, -1.0f, 1.0f)
		};

		for (std::size_t i = 0; i < kFrustumCorners.size(); i++)
		{
			glm::vec4 v = invViewProj * kFrustumCorners[i];
			v = v / v.w;
			corners.push_back(glm::vec3(v.x, v.y, v.z));
		}
	}

	inline float classify(const Plane& plane, const AABB& aabb)
	{
		glm::vec3 center = (aabb.max + aabb.min) / 2.0f;
		glm::vec3 extents = aabb.max - aabb.min;

		float r = fabsf(extents.x * plane.n.x) +
			fabsf(extents.y * plane.n.y) +
			fabsf(extents.z * plane.n.z);

		float d = glm::dot(plane.n, center) + plane.d;

		if (fabsf(d) < r)
			return 0.0f;
		else if (d < 0.0f)
			return d + r;
		else
			return d - r;
	}


	inline bool intersects(const Frustum& frustum, const AABB& aabb)
	{
		for (int i = 0; i < 6; i++)
		{
			if (classify(frustum.planes[i], aabb) < 0.0f)
				return false;
		}

		return true;
	}
}

#endif