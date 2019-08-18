#include "camera.h"

namespace es
{
	Camera* Camera::create()
	{
		Camera* camera = new (std::nothrow) Camera();
		camera->initDefault();
		camera->setDepth(0.0f);

		return camera;
	}

	Camera* Camera::createPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
	{
		Camera* camera = new (std::nothrow) Camera();
		if (camera)
		{
			camera->initPerspective(fov, aspectRatio, nearPlane, farPlane);
			return camera;
		}
		delete(camera);
		camera = nullptr;
		return nullptr;
	}

	Camera* Camera::createOrthographic(float zoomX, float zoomY, float nearPlane, float farPlane)
	{
		Camera* camera = new (std::nothrow) Camera();
		if (camera)
		{
			camera->initOrthoGraphic(zoomX, zoomY, nearPlane, farPlane);
			return camera;
		}
		delete(camera);
		camera = nullptr;
		return nullptr;
	}

	Camera* Camera::getDefaultCamera()
	{
		return World::getWorld()->getDefaultCamera();
	}

	Camera::Camera()
	{
		
	}

	Camera::~Camera()
	{
	
	}

	void Camera::lookAt(const glm::vec3& target, const glm::vec3& up)
	{

	}

	const glm::mat4& Camera::getProjectionMatrix() const
	{
		return projection;
	}

	const glm::mat4& Camera::getViewMatrix() const
	{
		return view;
	}

	const glm::mat4& Camera::getViewProjectionMatrix() const
	{
		if (viewProjectionDirty)
		{
			viewProjection = projection * view;
			viewProjectionDirty = false;
		}
		return viewProjection;
	}

	bool Camera::initDefault()
	{
		initPerspective(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
		setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		return true;
	}

	bool Camera::initPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
	{
		this->fov = fov;
		this->aspectRatio = aspectRatio;
		this->nearPlane = nearPlane;
		this->farPlane = farPlane;
		projection = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
		viewProjectionDirty = true;
		frustumDirty = true;
		type = Camera::Type::Perspective;

		return true;
	}

	bool Camera::initOrthoGraphic(float zoomX, float zoomY, float nearPlane, float farPlane)
	{
		this->zoom[0] = zoomX;
		this->zoom[1] = zoomY;
		this->nearPlane = nearPlane;
		this->farPlane = farPlane;
		projection = glm::ortho(0.0f, zoomX, 0.0f, zoomY, nearPlane, farPlane);
		viewProjectionDirty = true;
		frustumDirty = true;
		type = Camera::Type::Orthographic;

		return true;
	}

	void Camera::setDepth(int8_t depth)
	{
		if (this->depth != depth)
		{
			this->depth = depth;
		}
	}

	void Camera::update(float deltaTime)
	{
		if (viewProjectionDirty)
		{
			view = glm::mat4(1.0f);
			//glm::rotate(view, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			//glm::rotate(view, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			//glm::rotate(view, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			//glm::translate(view, position);
			viewProjectionDirty = false;
		}
	}

	void Camera::translate(const glm::vec3& deltaPosition)
	{
		this->position += deltaPosition;
		viewProjectionDirty = true;
	}

	void Camera::rotate(const glm::vec3& deltaEuler)
	{
		this->rotation += deltaEuler;
		viewProjectionDirty = true;
	}

	
}