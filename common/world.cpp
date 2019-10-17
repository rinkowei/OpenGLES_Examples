#include "world.h"

namespace es
{
	World* World::world = nullptr;
	World::GarbageDeleter World::gd;

	World::World()
	{
		mMainCamera = nullptr;
		mGlobalMaterial = nullptr;
		mIsGlobalMaterialEnabled = false;
	}

	World::~World()
	{
		if (mMainCamera != nullptr)
		{
			mMainCamera.reset(nullptr);
		}
		if (mGlobalMaterial != nullptr)
		{
			mGlobalMaterial.reset();
			mGlobalMaterial = nullptr;
		}
	}

	World* World::getWorld()
	{
		if (world == nullptr)
		{
			world = new (std::nothrow) World();
		}
		return world;
	}


	void World::enableGlobalMaterial(std::shared_ptr<Material> globalMaterial)
	{
		if (this->mGlobalMaterial != nullptr)
		{
			this->mGlobalMaterial.reset();
		}
		this->mGlobalMaterial = globalMaterial;
		//this->globalMaterial->apply();
		mIsGlobalMaterialEnabled = true;
	}

	void World::disableGlobalMaterial()
	{
		if (this->mGlobalMaterial != nullptr)
		{
			this->mGlobalMaterial.reset();
			this->mGlobalMaterial = nullptr;
		}
		mIsGlobalMaterialEnabled = false;
	}

	std::shared_ptr<Material> World::getGlobalMaterial() const
	{
		return mGlobalMaterial;
	}

	bool World::getGlobalMaterialEnabled() const
	{
		return mIsGlobalMaterialEnabled;
	}

	void World::update(float deltaTime)
	{
		if (mMainCamera != nullptr)
		{
			mMainCamera->update();
		}
	}

	void World::createMainCamera(float fov, float near, float far, float aspectRatio, glm::vec3 position, glm::vec3 forward)
	{
		if (mMainCamera != nullptr)
		{
			mMainCamera.reset(nullptr);
		}
		mMainCamera = Camera::create(fov, near, far, aspectRatio, position, forward);
	}

	Camera* World::getMainCamera() const
	{
		return mMainCamera.get();
	}
}
