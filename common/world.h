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

		class GarbageDeleter
		{
		public:
			GarbageDeleter() = default;
			~GarbageDeleter()
			{
				if (World::world)
				{
					delete World::world;
					World::world = nullptr;
				}
			}
		};
		static GarbageDeleter gd;
	};
}