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
			for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
			{
				delete(iter->second);
			}
			meshes.swap(std::unordered_map<std::string, Mesh*>());
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

			for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
			{
				if (singleMaterial.has_value())
				{
					singleMaterial.value()->apply();
					singleMaterial.value()->setMatrix4x4("model", model);
					singleMaterial.value()->setMatrix4x4("view", World::getWorld()->getDefaultCamera()->getViewMatrix());
					singleMaterial.value()->setMatrix4x4("projection", World::getWorld()->getDefaultCamera()->getProjectionMatrix());
				}
				else
				{
					iter->second->setModelMatrix(model);
				}
				iter->second->render(deltaTime);
			}
		}

		virtual void update(float deltaTime) override
		{
			Object::update(deltaTime);

			for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
			{
				iter->second->setModelMatrix(model);
			}
		}

		std::unordered_map<std::string, Mesh*> getMeshes() const
		{
			return meshes;
		}

		void setBoolean(const std::string& name, bool value) const
		{
			for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
			{
				iter->second->getMaterial()->setBoolean(name, value);
			}
		}

		void setInteger(const std::string& name, int value) const
		{
			for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
			{
				iter->second->getMaterial()->setInteger(name, value);
			}
		}

		void setFloat(const std::string& name, float value) const
		{
			for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
			{
				iter->second->getMaterial()->setFloat(name, value);
			}
		}

		void setVector2(const std::string& name, const glm::vec2& value) const
		{
			for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
			{
				iter->second->getMaterial()->setVector2(name, value);
			}
		}
		void setVector2(const std::string& name, float x, float y) const
		{
			for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
			{
				iter->second->getMaterial()->setVector2(name, x, y);
			}
		}

		void setVector3(const std::string& name, const glm::vec3& value) const
		{
			if (singleMaterial.has_value())
			{
				singleMaterial.value()->setVector3(name, value);
			}
			else
			{
				for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
				{
					iter->second->getMaterial()->setVector3(name, value);
				}
			}
		}
		void setVector3(const std::string& name, float x, float y, float z) const
		{
			for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
			{
				iter->second->getMaterial()->setVector3(name, x, y, z);
			}
		}

		void setVector4(const std::string& name, const glm::vec4& value) const
		{
			for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
			{
				iter->second->getMaterial()->setVector4(name, value);
			}
		}
		void setVector4(const std::string& name, float x, float y, float z, float w)
		{
			for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
			{
				iter->second->getMaterial()->setVector4(name, x, y, z, w);
			}
		}

		void setMatrix2x2(const std::string& name, const glm::mat2& mat) const
		{
			for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
			{
				iter->second->getMaterial()->setMatrix2x2(name, mat);
			}
		}

		void setMatrix3x3(const std::string& name, const glm::mat3& mat) const
		{
			for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
			{
				iter->second->getMaterial()->setMatrix3x3(name, mat);
			}
		}

		void setMatrix4x4(const std::string& name, const glm::mat4& mat) const
		{
			for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
			{
				iter->second->getMaterial()->setMatrix4x4(name, mat);
			}
		}

		void setSingleMaterial(std::shared_ptr<Material> singleMaterial)
		{
			this->singleMaterial = singleMaterial;
		}

		std::shared_ptr<Material> getSingleMaterial() const
		{
			if (singleMaterial.has_value())
			{
				return singleMaterial.value();
			}
			return nullptr;
		}
	private:
		std::unordered_map<std::string, Mesh*> meshes;
		std::optional<std::shared_ptr<Material>> singleMaterial;
		std::string directory;

		GLboolean loadWithFile(const std::string& path, const std::unordered_map<Material::ShaderType, std::string>& shader)
		{
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices | aiProcess_GenSmoothNormals);
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				std::cout << importer.GetErrorString() << std::endl;
				return false;
			}
			this->directory = path.substr(0, path.find_last_of('/'));

			handleNode(scene->mRootNode, scene, shader);

			return true;
		}

		void handleNode(aiNode* node, const aiScene* scene, const std::unordered_map<Material::ShaderType, std::string>& shader)
		{
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				meshes.insert(std::make_pair(std::string(mesh->mName.C_Str()), handleMesh(mesh, scene, shader)));
			}

			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				handleNode(node->mChildren[i], scene, shader);
			}
		}

		Mesh* handleMesh(aiMesh* mesh, const aiScene* scene, const std::unordered_map<Material::ShaderType, std::string>& shader)
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
					vertex.Color = glm::vec3(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b);
				else
					vertex.Color = glm::vec3(0.0f, 0.0f, 0.0f);
				

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
			
			// load diffuse maps
			std::vector<std::pair<Texture::Type, std::string>> diffuseMaps = loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			// load specular maps
			std::vector<std::pair<Texture::Type, std::string>> specularMaps = loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_SPECULAR);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			// load ambient maps
			std::vector<std::pair<Texture::Type, std::string>> ambientMaps = loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_AMBIENT);
			textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());

			// load emissive maps
			std::vector<std::pair<Texture::Type, std::string>> emissiveMaps = loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_EMISSIVE);
			textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());

			// load normal maps
			std::vector<std::pair<Texture::Type, std::string>> normalMaps = loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_NORMALS);
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

			// load height maps
			std::vector<std::pair<Texture::Type, std::string>> heightMaps = loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_HEIGHT);
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

			// load shininess maps
			std::vector<std::pair<Texture::Type, std::string>> shininessMaps = loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_SHININESS);
			textures.insert(textures.end(), shininessMaps.begin(), shininessMaps.end());

			// load opacity maps
			std::vector<std::pair<Texture::Type, std::string>> opacityMaps = loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_OPACITY);
			textures.insert(textures.end(), opacityMaps.begin(), opacityMaps.end());

			// load displacement maps
			std::vector<std::pair<Texture::Type, std::string>> displacementMaps = loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_DISPLACEMENT);
			textures.insert(textures.end(), displacementMaps.begin(), displacementMaps.end());

			// load light maps
			std::vector<std::pair<Texture::Type, std::string>> lightMaps = loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_LIGHTMAP);
			textures.insert(textures.end(), lightMaps.begin(), lightMaps.end());

			// load reflection maps
			std::vector<std::pair<Texture::Type, std::string>> reflectionMaps = loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_REFLECTION);
			textures.insert(textures.end(), reflectionMaps.begin(), reflectionMaps.end());

			// load unknown maps
			std::vector<std::pair<Texture::Type, std::string>> unknownMaps = loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_UNKNOWN);
			textures.insert(textures.end(), unknownMaps.begin(), unknownMaps.end());

			std::shared_ptr<Material> material = nullptr;
			if (!shader.empty())
			{
				material = std::make_shared<Material>(shader, textures);
			}

			Mesh* childMesh = Mesh::createWithData(vertices, indices, material, Mesh::DrawType::ELEMENTS);
			childMesh->setName(std::string(mesh->mName.C_Str()));
			childMesh->setAutoUpdated(false);
	
			return childMesh;
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
						texType = Texture::Type::DIFFUSE;
						break;
					}
					case aiTextureType_SPECULAR:
					{
						texType = Texture::Type::SPECULAR;
						break;
					}
					case aiTextureType_AMBIENT:
					{
						texType = Texture::Type::AMBIENT;
						break;
					}
					case aiTextureType_EMISSIVE:
					{
						texType = Texture::Type::EMISSIVE;
						break;
					}
					case aiTextureType_NORMALS:
					{
						texType = Texture::Type::NORMAL;
						break;
					}
					case aiTextureType_HEIGHT:
					{
						texType = Texture::Type::HEIGHT;
						break;
					}
					case aiTextureType_SHININESS:
					{
						texType = Texture::Type::SHININESS;
						break;
					}
					case aiTextureType_OPACITY:
					{
						texType = Texture::Type::OPACITY;
						break;
					}
					case aiTextureType_DISPLACEMENT:
					{
						texType = Texture::Type::DISPLACEMENT;
						break;
					}
					case aiTextureType_LIGHTMAP:
					{
						texType = Texture::Type::LIGHTMAP;
						break;
					}
					case aiTextureType_REFLECTION:
					{
						texType = Texture::Type::REFLECTION;
						break;
					}
					case aiTextureType_UNKNOWN:
					{
						texType = Texture::Type::UNKNOWN;
						break;
					}
				}

				textures.push_back(std::make_pair(texType, texPath));
			}
			return textures;
		}
	};
}

