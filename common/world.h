#pragma once

#include <iostream>
#include <memory>
#include "camera.h"

namespace es
{
	class World
	{
	public:
		World();
		~World();

		static World* getWorld();

		Camera* getMainCamera();
		
	private:
		std::unique_ptr<Camera> mainCamera;
		static World* world;
	};
}