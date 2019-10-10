#ifndef FRUSTUM_H_
#define FRUSTUM_H_

#include <iostream>
#include <array>
#include "plane.h"

namespace es
{
	class Camera;

	class Frustum
	{
		friend class Camera;
	public:
		// constructor
		Frustum();
		
		// destructor
		~Frustum();

		// init frustum from camera
		bool initFrustum(const Camera* camera);

		void setClipZ(GLboolean clipZ);

		GLboolean getClipZ() const;
	protected:
		// create clip plane
		void createPlane(const Camera* camera);

		// clip plane included left, right, top, bottom, near, far
		std::array<Plane, 6> plane;
		GLboolean clipZ;
		GLboolean initialized;
	};
}

#endif