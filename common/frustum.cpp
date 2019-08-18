#include "frustum.h"
#include "camera.h"

namespace es
{
	Frustum::Frustum()
		:clipZ(true),
		 initialized(false)
	{

	}

	Frustum::~Frustum()
	{

	}

	bool Frustum::initFrustum(const Camera* camera)
	{
		initialized = true;
		createPlane(camera);
		return true;
	}

	void Frustum::createPlane(const Camera* camera)
	{
		glm::mat4 mat = camera->getViewProjectionMatrix();
		plane[0].initPlane(-glm::vec3(mat[0][3] + mat[0][0], mat[1][3] + mat[1][0], mat[2][3] + mat[2][0]), (mat[3][3] + mat[3][0]));//left
		plane[1].initPlane(-glm::vec3(mat[0][3] - mat[0][0], mat[1][3] - mat[1][0], mat[2][3] - mat[2][0]), (mat[3][3] - mat[3][0]));//right
		plane[2].initPlane(-glm::vec3(mat[0][3] + mat[0][1], mat[1][3] + mat[1][1], mat[2][3] + mat[2][1]), (mat[3][3] + mat[3][1]));//bottom
		plane[3].initPlane(-glm::vec3(mat[0][3] - mat[0][1], mat[1][3] - mat[1][1], mat[2][3] - mat[2][1]), (mat[3][3] - mat[3][1]));//top
		plane[4].initPlane(-glm::vec3(mat[0][3] + mat[0][2], mat[1][3] + mat[1][2], mat[2][3] + mat[2][2]), (mat[3][3] + mat[3][2]));//near
		plane[5].initPlane(-glm::vec3(mat[0][3] - mat[0][2], mat[1][3] - mat[1][2], mat[2][3] - mat[2][2]), (mat[3][3] - mat[3][2]));//far
	}
}