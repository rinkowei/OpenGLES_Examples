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
		Model(const std::string& name, const std::string& path, const std::vector<std::string>& shaderFiles, bool isLoadMaterials = true);
		Model(const std::string& name, const Model* duplicateModel);
		~Model();

		static std::shared_ptr<Model> createFromFile(const std::string& name, const std::string& path, const std::vector<std::string>& shaderFiles, bool isLoadMaterials = true);

		static std::shared_ptr<Model> clone(const std::string& name, const Model* duplicateModel);

		void setMaterial(std::shared_ptr<Material> mMat);

		void render(bool isUseLocalMaterial = true);

		template<typename T>
		void setUniform(const std::string& name, const T& value)
		{
			for (auto iter = mMeshes.begin(); iter != mMeshes.end(); iter++)
			{
				iter->second->setUniform(name, value);
			}
		}
	private:
		void handleNode(aiNode* node, const aiScene* scene, bool isLoadMaterials);
		std::shared_ptr<Mesh> handleMesh(aiMesh* mesh, const aiScene* scene, bool isLoadMaterials);
		std::shared_ptr<Material> handleMaterial(aiMesh* mesh, const aiScene* scene);

		std::string mDirectory;

		std::vector<std::string> mShaderFiles;
		std::shared_ptr<Material> mMaterial;

		std::map<std::string, std::shared_ptr<Mesh>> mMeshes;

		static std::array<std::string, 11> kTextureTypeStrings;

		static std::unordered_map<std::string, std::shared_ptr<Model>> mModelCache;
	};
}

#endif