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

#include "mesh.h"
#include "world.h"

namespace es
{
	class Model : public Object
	{
	public:
		Model();
		~Model();

		static Model* createWithFile(const std::string& path, const std::unordered_map<Material::ShaderType, std::string>& shaders);

		virtual GLvoid render(float deltaTime) override;

		virtual GLvoid update(float deltaTime) override;

		std::unordered_map<std::string, Mesh*> getMeshes() const;

		GLvoid setBoolean(const std::string& name, bool value) const;

		GLvoid setInteger(const std::string& name, int value) const;

		GLvoid setFloat(const std::string& name, float value) const;

		GLvoid setVector2(const std::string& name, const glm::vec2& value) const;

		GLvoid setVector2(const std::string& name, float x, float y) const;

		GLvoid setVector3(const std::string& name, const glm::vec3& value) const;

		GLvoid setVector3(const std::string& name, float x, float y, float z) const;

		GLvoid setVector4(const std::string& name, const glm::vec4& value) const;

		GLvoid setVector4(const std::string& name, float x, float y, float z, float w) const;

		GLvoid setMatrix2x2(const std::string& name, const glm::mat2& mat) const;

		GLvoid setMatrix3x3(const std::string& name, const glm::mat3& mat) const;

		GLvoid setMatrix4x4(const std::string& name, const glm::mat4& mat) const;

	private:
		std::unordered_map<std::string, Mesh*> meshes;
		std::string directory;

		GLboolean loadWithFile(const std::string& path, const std::unordered_map<Material::ShaderType, std::string>& shader);

		GLvoid handleNode(aiNode* node, const aiScene* scene, const std::unordered_map<Material::ShaderType, std::string>& shader);

		Mesh* handleMesh(aiMesh* mesh, const aiScene* scene, const std::unordered_map<Material::ShaderType, std::string>& shader);

		std::vector<std::pair<Texture::Type, std::string>> loadMaterialTextures(aiMaterial* mat, aiTextureType type);
	};
}

