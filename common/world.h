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

		Camera* getDefaultCamera() const;
		
	private:
		Camera* defaultCamera;
		static World* world;
	};
}