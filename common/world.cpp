
#include "world.h"

namespace es
{
	World* World::world = nullptr;
	World::GarbageDeleter World::gd;

	World::World()
	{
		defaultCamera = Camera::create();
		globalMaterial = nullptr;
		isGlobalMaterialEnabled = false;
	}

	World::~World()
	{
		delete(defaultCamera);
		defaultCamera = nullptr;
	}

	World* World::getWorld()
	{
		if (world == nullptr)
		{
			world = new (std::nothrow) World();
		}
		return world;
	}

	Camera* World::getDefaultCamera() const
	{
		return defaultCamera;
	}

	GLvoid World::enableGlobalMaterial(std::shared_ptr<Material> globalMaterial)
	{
		if (this->globalMaterial != nullptr)
		{
			this->globalMaterial.reset();
		}
		this->globalMaterial = globalMaterial;
		//this->globalMaterial->apply();
		isGlobalMaterialEnabled = true;
	}

	GLvoid World::disableGlobalMaterial()
	{
		if (this->globalMaterial != nullptr)
		{
			this->globalMaterial.reset();
			this->globalMaterial = nullptr;
		}
		isGlobalMaterialEnabled = false;
	}

	std::shared_ptr<Material> World::getGlobalMaterial() const
	{
		return globalMaterial;
	}

	GLboolean World::getGlobalMaterialEnabled() const
	{
		return isGlobalMaterialEnabled;
	}
}
