#ifndef WORLD_H_
#define WORLD_H_

#include <iostream>
#include <memory>

#include "camera.h"
#include "material.h"

namespace es
{
	class World
	{
	public:
		World();
		~World();

		static World* getWorld();
		
		GLvoid enableGlobalMaterial(std::shared_ptr<Material> globalMaterial);

		GLvoid disableGlobalMaterial();

		std::shared_ptr<Material> getGlobalMaterial() const;

		GLboolean getGlobalMaterialEnabled() const;
	private:

		std::shared_ptr<Material> globalMaterial;
		GLboolean isGlobalMaterialEnabled;

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

#endif