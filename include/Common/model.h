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

#include <Common/mesh.h>
#include <Common/shader.h>
#include <Common/textureLoader.h>

using namespace std;

class Model
{
public:
	std::vector<Texture> texturesLoaded;
	std::vector<Mesh> meshes;

	std::string fileName;
	bool gammaCorrection;

	Model(const std::string& path, bool gamma = false)
		: gammaCorrection(gamma)
	{
		loadModel(path);
	}

	void Draw(Shader shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].Draw(shader);
		}
	}

private:
	void loadModel(const std::string& filePath)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			throw std::runtime_error("error : " + std::string(importer.GetErrorString()));
			return;
		}
		fileName = filePath;

		handleNode(scene->mRootNode, scene);
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
		std::vector<Texture> textures;
	
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			
			if (mesh->HasPositions())
			{
				vertex.Position.x = mesh->mVertices[i].x;
				vertex.Position.y = mesh->mVertices[i].y;
				vertex.Position.z = mesh->mVertices[i].z;
			}
			else
			{
				vertex.Position = glm::vec3(0.0f);
			}

			if (mesh->HasNormals())
			{
				vertex.Normal.x = mesh->mNormals[i].x;
				vertex.Normal.y = mesh->mNormals[i].y;
				vertex.Normal.z = mesh->mNormals[i].z;
			}
			else
			{
				vertex.Normal = glm::vec3(0.0f);
			}

			if (mesh->HasTextureCoords(0))
			{
				vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
				vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
			}
			else
			{
				vertex.TexCoords = glm::vec2(0.0f);
			}

			if (mesh->HasTangentsAndBitangents())
			{
				vertex.Tangent.x = mesh->mTangents[i].x;
				vertex.Tangent.y = mesh->mTangents[i].y;
				vertex.Tangent.z = mesh->mTangents[i].z;

				vertex.Bitangent.x = mesh->mBitangents[i].x;
				vertex.Bitangent.y = mesh->mBitangents[i].y;
				vertex.Bitangent.z = mesh->mBitangents[i].z;
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
		
		std::vector<Texture> diffuseTextures = loadMaterialTextures(material, aiTextureType_DIFFUSE, "textureDiffuse");
		textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());
		/*
		std::vector<Texture> specularTextures = loadMaterialTextures(material, aiTextureType_SPECULAR, "textureSpecular");
		textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());

		std::vector<Texture> normalTextures = loadMaterialTextures(material, aiTextureType_NORMALS, "textureNormals");
		textures.insert(textures.end(), normalTextures.begin(), normalTextures.end());

		std::vector<Texture> heightTextures = loadMaterialTextures(material, aiTextureType_HEIGHT, "textureHeight");
		textures.insert(textures.end(), heightTextures.begin(), heightTextures.end());
		*/
		return Mesh(vertices, indices, textures);
	}

	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName)
	{
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);

			bool alreadyLoaded = false;
			for (unsigned int j = 0; j < texturesLoaded.size(); j++)
			{
				if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(texturesLoaded[j]);
					alreadyLoaded = true;
					break;
				}
			}
			if (!alreadyLoaded)
			{
				Texture tex;
				tex.id = textureLoader::loadTexture2D(string(str.C_Str()), true);
				tex.type = typeName;
				tex.path = str.C_Str();
				textures.push_back(tex);
				texturesLoaded.push_back(tex);
			}
		}
		return textures;
	}
};

