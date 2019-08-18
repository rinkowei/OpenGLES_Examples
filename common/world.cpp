
#include "world.h"

namespace es
{
	World* World::world = nullptr;

	World::World()
	{
		defaultCamera = Camera::create();
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
}
