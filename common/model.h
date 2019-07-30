#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <common.h>

using namespace std;
using namespace es;

class Model
{
public:
	Model() = default;
	~Model() = default;

	static Model* createWithFile(const std::string& path, bool gamma = false)
	{
		Model* model = new (std::nothrow) Model();
		if (model && model->loadWithFile(path, gamma))
		{
			return model;
		}
		delete(model);
		return nullptr;
	}

	void Draw(Shader shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].Draw();
		}
	}

private:
	std::vector<Texture*> texturesLoaded;
	std::vector<Mesh> meshes;

	std::string directory;
	bool gammaCorrection = false;

	bool loadWithFile(const std::string& path, bool gamma)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << importer.GetErrorString() << std::endl;
			return false;
		}
		directory = path.substr(0, path.find_last_of('/'));

		handleNode(scene->mRootNode, scene);

		return true;
	}

	void handleNode(aiNode* node, const aiScene* scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(handleMesh(mesh, scene));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			handleNode(node->mChildren[i], scene);
		}
	}

	Mesh handleMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Texture*> textures;
	
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			
			if (mesh->HasPositions())
			{
				vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			}
			else
			{
				vertex.Position = glm::vec3(0.0f);
			}

			if (mesh->HasNormals())
			{
				vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			}
			else
			{
				vertex.Normal = glm::vec3(0.0f);
			}

			if (mesh->HasTextureCoords(0))
			{
				vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			}
			else
			{
				vertex.TexCoords = glm::vec2(0.0f);
			}

			if (mesh->HasTangentsAndBitangents())
			{
				vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
				vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
			}
			else
			{
				vertex.Tangent = glm::vec3(0.0f);
				vertex.Bitangent = glm::vec3(0.0f);
			}
			
			vertices.push_back(vertex);
		}

		if (mesh->HasFaces())
		{
			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++)
				{
					indices.push_back(face.mIndices[j]);
				}
			}
		}
		
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		
		std::vector<Texture*> diffuseTextures = loadMaterialTextures(material, aiTextureType_DIFFUSE, Texture::Type::Diffuse);
		textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());
		
		std::vector<Texture*> specularTextures = loadMaterialTextures(material, aiTextureType_SPECULAR, Texture::Type::Specular);
		textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());

		std::vector<Texture*> normalTextures = loadMaterialTextures(material, aiTextureType_NORMALS, Texture::Type::Normal);
		textures.insert(textures.end(), normalTextures.begin(), normalTextures.end());

		std::vector<Texture*> heightTextures = loadMaterialTextures(material, aiTextureType_HEIGHT, Texture::Type::Height);
		textures.insert(textures.end(), heightTextures.begin(), heightTextures.end());
		
		return *Mesh::createWithData(vertices, indices, textures, Mesh::DrawType::Elements);
	}

	std::vector<Texture*> loadMaterialTextures(aiMaterial* mat, aiTextureType type, Texture::Type texType)
	{
		std::vector<Texture*> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			
			std::string texName = std::string(str.C_Str());
			if (texName.find('/') != texName.npos)
			{
				texName = texName.substr(texName.find_last_of('/') + 1);
			}
			else if (texName.find('\\') != texName.npos)
			{
				texName = texName.substr(texName.find_last_of('\\') + 1);
			}

			std::string texPath = directory + '/' + texName;

			bool alreadyLoaded = false;
			for (unsigned int j = 0; j < texturesLoaded.size(); j++)
			{
				if (std::strcmp(texturesLoaded[j]->getFilePath().data(), str.C_Str()) == 0)
				{
					textures.push_back(texturesLoaded[j]);
					alreadyLoaded = true;
					break;
				}
			}
			if (!alreadyLoaded)
			{
				Texture* tex = Texture::createWithFile(texPath, texType);
				textures.push_back(tex);
				texturesLoaded.push_back(tex);
			}
		}
		return textures;
	}
};

