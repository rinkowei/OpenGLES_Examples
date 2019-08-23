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

#include "world.h"

using namespace std;

namespace es
{
	class Model : public Object
	{
	public:
		Model() = default;
		~Model()
		{
			for (size_t i = 0; i < meshes.size(); i++)
			{
				delete(meshes[i]);
			}
			meshes.swap(std::vector<Mesh*>());
		}

		static Model* createWithFile(const std::string& path, const std::unordered_map<Material::ShaderType, std::string>& shaders)
		{
			Model* model = new (std::nothrow) Model();
			if (model && model->loadWithFile(path, shaders))
			{
				return model;
			}
			delete(model);
			return nullptr;
		}

		virtual void render(float deltaTime) override
		{
			if (autoUpdated)
			{
				Object::update(deltaTime);
			}

			for (unsigned int i = 0; i < meshes.size(); i++)
			{
				meshes[i]->setModelMatrix(model);
				meshes[i]->render(deltaTime);
			}
		}

		virtual void update(float deltaTime) override
		{
			Object::update(deltaTime);
			for (size_t i = 0; i < meshes.size(); i++)
			{
				meshes[i]->setModelMatrix(model);
			}
		}

	private:
		std::vector<Mesh*> meshes;
		std::unordered_map<Material::ShaderType, std::string> shaders;
		std::string directory;

		bool loadWithFile(const std::string& path, const std::unordered_map<Material::ShaderType, std::string>& shaders)
		{
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices);
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				std::cout << importer.GetErrorString() << std::endl;
				return false;
			}
			this->directory = path.substr(0, path.find_last_of('/'));

			this->shaders = shaders;

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

		Mesh* handleMesh(aiMesh* mesh, const aiScene* scene)
		{
			std::vector<Vertex> vertices;
			std::vector<GLuint> indices;
			std::vector<std::pair<Texture::Type, std::string>> textures;
			
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				Vertex vertex;

				if (mesh->HasPositions())
					vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
				else
					vertex.Position = glm::vec3(0.0f);

				if (mesh->HasTextureCoords(0))
					vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
				else
					vertex.TexCoords = glm::vec2(0.0f);

				if (mesh->HasNormals())
					vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
				else
					vertex.Normal = glm::vec3(0.0f);

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

				if (mesh->HasVertexColors(0))
					vertex.Color = glm::vec4(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b, mesh->mColors[0][i].a);
				else
					vertex.Color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

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
			
			std::vector<std::pair<Texture::Type, std::string>> diffuseMaps = loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<std::pair<Texture::Type, std::string>> specularMaps = loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_SPECULAR);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			std::vector<std::pair<Texture::Type, std::string>> normalMaps = loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_NORMALS);
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

			std::vector<std::pair<Texture::Type, std::string>> heightMaps = loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_HEIGHT);
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

			std::shared_ptr<Material> material = std::make_shared<Material>(shaders, textures);

			Mesh* partMesh = Mesh::createWithData(vertices, indices, Mesh::DrawType::Elements, material);
			partMesh->setAutoUpdated(false);

			return partMesh;
		}

		std::vector<std::pair<Texture::Type, std::string>> loadMaterialTextures(aiMaterial* mat, aiTextureType type)
		{
			std::vector<std::pair<Texture::Type, std::string>> textures;
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

				Texture::Type texType;
				switch (type)
				{
					case aiTextureType_DIFFUSE:
					{
						texType = Texture::Type::Diffuse;
						break;
					}
					case aiTextureType_SPECULAR:
					{
						texType = Texture::Type::Specular;
						break;
					}
					case aiTextureType_NORMALS:
					{
						texType = Texture::Type::Normal;
						break;
					}
					case aiTextureType_HEIGHT:
					{
						texType = Texture::Type::Height;
						break;
					}
				}

				textures.push_back(std::make_pair(texType, texPath));
			}
			return textures;
		}
	};
}

