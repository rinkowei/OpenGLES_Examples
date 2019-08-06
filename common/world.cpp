
#include "world.h"

using namespace es;

World* World::world = nullptr;

World::World()
{
	mainCamera = std::make_unique<Camera>();
}

World::~World()
{

}

World* World::getWorld()
{
	if (world == nullptr)
	{
		world = new (std::nothrow) World();
	}
	return world;
}

Camera* World::getMainCamera()
{
	return mainCamera.get();
}
