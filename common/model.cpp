#include "model.h"

namespace es
{
	std::array<std::string, 11> Model::kTextureTypeStrings =
	{
		"diffuseMap",
		"specularMap",
		"ambientMap",
		"emissiveMap",
		"heightMap",
		"normalsMap",
		"shininessMap",
		"opacityMap",
		"displacementMap",
		"lightMap",
		"reflectionMap"
	};

	std::unordered_map<std::string, std::shared_ptr<Model>> Model::mModelCache;

	Model::Model(const std::string& name, const std::string& path, const std::vector<std::string>& shaderFiles, bool isLoadMaterials) : Object(name)
	{
		const aiScene* scene;
		Assimp::Importer importer;
		scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, importer.GetErrorString());
			return;
		}

		mDirectory = Utility::pathWithoutFile(path);
		mShaderFiles = shaderFiles;

		handleNode(scene->mRootNode, scene, isLoadMaterials);
	}

	Model::~Model()
	{
		if (mMaterial != nullptr)
		{
			mMaterial.reset();
			mMaterial = nullptr;
		}

		for (auto iter = mMeshes.begin(); iter != mMeshes.end(); iter++)
		{
			if (iter->second != nullptr)
			{
				iter->second.reset();
				iter->second = nullptr;
			}
		}
		mMeshes.swap(std::map<std::string, std::shared_ptr<Mesh>>());
	}

	std::shared_ptr<Model> Model::createFromFile(const std::string& name, const std::string& path, const std::vector<std::string>& shaderFiles, bool isLoadMaterials)
	{
		if (mModelCache.find(name) == mModelCache.end())
		{
			std::shared_ptr<Model> model = std::make_shared<Model>(name, path, shaderFiles, isLoadMaterials);
			mModelCache[name] = model;
			return model;
		}
		else
		{
			return mModelCache[name];
		}
	}

	void Model::setMaterial(std::shared_ptr<Material> mMat)
	{
		if (mMaterial != nullptr)
		{
			mMaterial.reset();
		}
		mMaterial = mMat;

		for (auto iter = mMeshes.begin(); iter != mMeshes.end(); iter++)
		{
			iter->second->setMaterial(mMat);
		}
	}

	void Model::render(bool isUseLocalMaterial)
	{
		if (mAutoUpdated)
		{
			Object::update();
		}

		for (auto iter = mMeshes.begin(); iter != mMeshes.end(); iter++)
		{
			iter->second->setModelMatrix(mModelMatrix);
			iter->second->render(isUseLocalMaterial);
		}
	}

	void Model::handleNode(aiNode* node, const aiScene* scene, bool isLoadMaterials)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			mMeshes.insert(std::make_pair(std::string(mesh->mName.C_Str()), handleMesh(mesh, scene, isLoadMaterials)));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			handleNode(node->mChildren[i], scene, isLoadMaterials);
		}
	}

	std::shared_ptr<Mesh> Model::handleMesh(aiMesh* mesh, const aiScene* scene, bool isLoadMaterials)
	{
		std::vector<Vertex> vertices(mesh->mNumVertices);
		std::vector<uint32_t> indices(mesh->mNumFaces * 3);

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			// handle vertex positions
			if (mesh->HasPositions())
			{
				vertices.at(i).vPosition = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			}

			// handle vertex texture coords
			if (mesh->HasTextureCoords(0))
			{
				vertices.at(i).vTexcoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			}

			// handle vertex normals
			if (mesh->HasNormals())
			{
				vertices.at(i).vNormal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			}

			// handle vertex tangents and bitangents
			if (mesh->HasTangentsAndBitangents())
			{
				vertices.at(i).vTangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
				vertices.at(i).vBitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
			}

			// handle vertex colors
			if (mesh->HasVertexColors(0))
			{
				vertices.at(i).vColor = glm::vec4(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b, mesh->mColors[0][i].a);
			}
		}

		if (mesh->HasFaces())
		{
			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++)
				{
					indices.at(i * 3 + j) = (face.mIndices[j]);
				}
			}
		}

		std::shared_ptr<Mesh> subMesh = Mesh::createWithData(std::string(mesh->mName.C_Str()), vertices, indices);
		if (indices.size() > 0)
		{
			subMesh->setDrawType(Mesh::DrawType::ELEMENTS);
		}
		else
		{
			subMesh->setDrawType(Mesh::DrawType::ARRAYS);
		}
		subMesh->setAutoUpdated(false);

		if (isLoadMaterials)
		{
			std::shared_ptr<Material> mat = handleMaterial(mesh, scene);
			subMesh->setMaterial(mat);
		}

		return subMesh;
	}

	std::shared_ptr<Material> Model::handleMaterial(aiMesh* mesh, const aiScene* scene)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		aiTextureType texType = aiTextureType::aiTextureType_DIFFUSE;

		std::unordered_map<std::string, std::string> textureFiles;

		for (std::size_t i = 0; i < kTextureTypeStrings.size(); i++)
		{
			unsigned int texCount = material->GetTextureCount(texType);
			for (unsigned int j = 0; j < texCount; j++)
			{
				aiString str;
				material->GetTexture(texType, j, &str);
				
				std::string texName = Utility::pathWithoutFile(std::string(str.C_Str()));
				textureFiles.insert(std::make_pair(kTextureTypeStrings[i] + "_" + std::to_string(j), mDirectory + "/" + texName));
			}
			texType = static_cast<aiTextureType>(texType + 1);
		}

		std::shared_ptr<Program> singleProgram = Program::createFromFiles(mName + "_program", mShaderFiles);
		std::shared_ptr<Material> subMaterial = Material::createFromProgram(mName + "_" + std::string(mesh->mName.C_Str()) + "_mat", singleProgram, textureFiles);
		return subMaterial;
	}
}