#ifndef WORLD_H_
#define WORLD_H_

#include <glm/glm.hpp>

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
		
		void enableGlobalMaterial(std::shared_ptr<Material> globalMaterial);

		void disableGlobalMaterial();

		std::shared_ptr<Material> getGlobalMaterial() const;

		bool getGlobalMaterialEnabled() const;

		void update(float deltaTime);

		void createMainCamera(float fov, float near, float far, float aspectRatio, glm::vec3 position, glm::vec3 forward);

		Camera* getMainCamera() const;
	private:
		std::unique_ptr<Camera> mMainCamera;
		std::shared_ptr<Material> mGlobalMaterial;
		bool mIsGlobalMaterialEnabled;

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