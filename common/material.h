#pragma once

#include "ogles.h"
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>

#include "texture.h"

namespace es
{
	class Material : public std::enable_shared_from_this<Material>
	{
	public:
		enum class ShaderType
		{
			VERTEX,                         // vertex shader
			TESSELLATION_CONTROL,           // tessellation control
			TESSELLATION_EVALUATION,        // tessellation evaluation
			GEOMETRY,                       // geometry shader
			FRAGMENT,                       // fragment shader
			PROGRAM                         // GLSL shader program
		};

		Material();

		Material(const std::unordered_map<Material::ShaderType, std::string>& shaderPaths, std::vector<std::pair<Texture::Type, std::string>>& texturePaths);

		~Material();

		std::shared_ptr<Material> getSharedPtr();

		static Material* createWithFile(const std::unordered_map<Material::ShaderType, std::string>& shaderPaths, std::vector<std::pair<Texture::Type, std::string>>& texturePaths);

		GLvoid apply();
		
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

		GLuint getProgramID();

	private:
		GLuint programID = 0;
		GLboolean isLinkSucceed = false;
		 
		std::vector<Texture*> textures;

		static std::set<Texture*> loadedTextures;
		
		GLboolean loadWithFile(const std::unordered_map<Material::ShaderType, std::string>& shaderPaths, std::vector<std::pair<Texture::Type, std::string>>& texturePaths);

		GLboolean loadShaders(const std::unordered_map<Material::ShaderType, std::string>& shaderPaths);

		GLboolean loadTextures(std::vector<std::pair<Texture::Type, std::string>>& texturePaths);

		GLboolean compileWithFile(Material::ShaderType type, const std::string& filePath, GLuint* shaderID);

		GLboolean compileWithSource(Material::ShaderType type, const GLchar* source, GLuint* shaderID);

		GLboolean checkCompiled(GLuint shader, ShaderType type);
	};
}