#ifndef MODEL_H_
#define MODEL_H_

#include <ogles.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <mesh.h>
#include <utility.h>

namespace es
{
	class Model : public Object
	{
	public:
		Model(const std::string& name, const std::string& path, bool loadMaterials = true);
		~Model();

		static std::shared_ptr<Model> createFromFile(const std::string& name, const std::string& path, bool loadMaterials = true);

		void setMaterial(std::shared_ptr<Material> mMat);

		void render(bool isUseLocalMaterial = true);
	private:
		void handleNode(aiNode* node, const aiScene* scene);
		std::shared_ptr<Mesh> handleMesh(aiMesh* mesh, const aiScene* scene);

		std::string mDirectory;
		std::shared_ptr<Material> mMaterial;
		std::map<std::string, std::shared_ptr<Mesh>> mMeshes;
		static std::unordered_map<std::string, std::shared_ptr<Model>> mModelCache;
	};
}

#endif