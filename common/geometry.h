#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <glm/glm.hpp>
#include <array>

namespace es
{
	struct Frustum
	{
		float mNear;
		float mFar;
		float mFov;
		float mAspectRatio;

		std::array<glm::vec3, 8> mFrustumCorners;
	};
}

#endif