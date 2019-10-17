#include "model.h"

namespace es
{
	std::unordered_map<std::string, std::shared_ptr<Model>> Model::mModelCache;

	Model::Model(const std::string& path, bool loadMaterials)
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

		handleNode(scene->mRootNode, scene);
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

	std::shared_ptr<Model> Model::createFromFile(const std::string& path, bool loadMaterials)
	{
		if (mModelCache.find(path) == mModelCache.end())
		{
			std::shared_ptr<Model> model = std::make_shared<Model>(path, loadMaterials);
			mModelCache[path] = model;
			return model;
		}
		else
		{
			return mModelCache[path];
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

	void Model::handleNode(aiNode* node, const aiScene* scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			mMeshes.insert(std::make_pair(std::string(mesh->mName.C_Str()), handleMesh(mesh, scene)));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			handleNode(node->mChildren[i], scene);
		}
	}

	std::shared_ptr<Mesh> Model::handleMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices(mesh->mNumVertices);
		std::vector<uint32_t> indices;

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
					indices.at(i + i * j) = (face.mIndices[j]);
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

		return subMesh;
	}
}