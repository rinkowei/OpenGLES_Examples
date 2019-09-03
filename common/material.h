#pragma once

#include <glad/glad.h>
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

using namespace es;

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

		Material() = default;

		Material(const std::unordered_map<Material::ShaderType, std::string>& shaderPaths, std::vector<std::pair<Texture::Type, std::string>>& texturePaths)
		{
			loadWithFile(shaderPaths, texturePaths);
		}

		~Material()
		{
			std::cout << "~Material\n";
			if (programID != 0 && isLinkSucceed)
			{
				glDeleteProgram(programID);
			}

			if (!textures.empty())
			{
				for (auto& texture : textures)
				{
					delete(texture);
				}
				textures.swap(std::vector<Texture*>());
			}
		}

		std::shared_ptr<Material> getSharedPtr()
		{
			return shared_from_this();
		}

		static Material* createWithFile(const std::unordered_map<Material::ShaderType, std::string>& shaderPaths, std::vector<std::pair<Texture::Type, std::string>>& texturePaths)
		{
			Material* material = new (std::nothrow) Material();
			if (material && material->loadWithFile(shaderPaths, texturePaths))
			{
				return material;
			}
			delete(material);
			return nullptr;
		}

		void apply()
		{
			for (size_t i = 0; i < textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, textures[i]->getID());
			}

			if (programID != 0 && isLinkSucceed)
			{
				glUseProgram(programID);
			}
		}

		void setBoolean(const std::string& name, bool value) const
		{
			glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
		}

		void setInteger(const std::string& name, int value) const
		{
			glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
		}

		void setFloat(const std::string& name, float value) const
		{
			glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
		}

		void setVector2(const std::string& name, const glm::vec2& value) const
		{
			glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
		}
		void setVector2(const std::string& name, float x, float y) const
		{
			glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y);
		}

		void setVector3(const std::string& name, const glm::vec3& value) const
		{
			glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
		}
		void setVector3(const std::string& name, float x, float y, float z) const
		{
			glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
		}

		void setVector4(const std::string& name, const glm::vec4& value) const
		{
			glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
		}
		void setVector4(const std::string& name, float x, float y, float z, float w)
		{
			glUniform4f(glGetUniformLocation(programID, name.c_str()), x, y, z, w);
		}

		void setMatrix2x2(const std::string& name, const glm::mat2& mat) const
		{
			glUniformMatrix2fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

		void setMatrix3x3(const std::string& name, const glm::mat3& mat) const
		{
			glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

		void setMatrix4x4(const std::string& name, const glm::mat4& mat) const
		{
			glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

		GLuint getProgramID()
		{
			return programID;
		}

	private:
		GLuint programID = 0;
		GLboolean isLinkSucceed = false;
		 
		std::vector<Texture*> textures;

		static std::set<Texture*> loadedTextures;
		
		bool loadWithFile(const std::unordered_map<Material::ShaderType, std::string>& shaderPaths, std::vector<std::pair<Texture::Type, std::string>>& texturePaths)
		{
			return loadShaders(shaderPaths) && loadTextures(texturePaths);
		}

		bool loadShaders(const std::unordered_map<Material::ShaderType, std::string>& shaderPaths)
		{
			if (programID != 0 && isLinkSucceed)
			{
				glDeleteProgram(programID);
			}

			programID = glCreateProgram();

			GLuint vertShaderID = 0;
			auto& shader = shaderPaths.find(Material::ShaderType::VERTEX);
			if (shader != shaderPaths.end())
			{
				if (!compileWithFile(Material::ShaderType::VERTEX, shader->second, &vertShaderID))
				{
					return false;
				}
				glAttachShader(programID, vertShaderID);
			}

			GLuint tessCtrlShaderID = 0;
			shader = shaderPaths.find(Material::ShaderType::TESSELLATION_CONTROL);
			if (shader != shaderPaths.end())
			{
				if (!compileWithFile(Material::ShaderType::TESSELLATION_CONTROL, shader->second, &tessCtrlShaderID))
				{
					return false;
				}
				glAttachShader(programID, tessCtrlShaderID);
			}

			GLuint tessEvalShaderID = 0;
			shader = shaderPaths.find(Material::ShaderType::TESSELLATION_EVALUATION);
			if (shader != shaderPaths.end())
			{
				if (!compileWithFile(Material::ShaderType::TESSELLATION_EVALUATION, shader->second, &tessEvalShaderID))
				{
					return false;
				}
				glAttachShader(programID, tessEvalShaderID);
			}

			GLuint geomShaderID = 0;
			shader = shaderPaths.find(Material::ShaderType::GEOMETRY);
			if (shader != shaderPaths.end())
			{
				if (!compileWithFile(Material::ShaderType::GEOMETRY, shader->second, &geomShaderID))
				{
					return false;
				}
				glAttachShader(programID, geomShaderID);
			}

			GLuint fragShaderID = 0;
			shader = shaderPaths.find(Material::ShaderType::FRAGMENT);
			if (shader != shaderPaths.end())
			{
				if (!compileWithFile(Material::ShaderType::FRAGMENT, shader->second, &fragShaderID))
				{
					return false;
				}
				glAttachShader(programID, fragShaderID);
			}

			glLinkProgram(programID);
			if (!checkCompiled(programID, Material::ShaderType::PROGRAM))
			{
				return false;
			}

			glDeleteShader(vertShaderID);
			glDeleteShader(tessCtrlShaderID);
			glDeleteShader(tessEvalShaderID);
			glDeleteShader(geomShaderID);
			glDeleteShader(fragShaderID);

			return true;
		}

		bool loadTextures(std::vector<std::pair<Texture::Type, std::string>>& texturePaths)
		{
			if (!texturePaths.empty())
			{
				std::sort(texturePaths.begin(), texturePaths.end(), [&](const std::pair<Texture::Type, std::string>& a, const std::pair<Texture::Type, std::string>& b) {
					return a.first < b.first;
				});
			}

			if (!textures.empty())
			{
				for (auto texture : textures)
				{
					delete(texture);
				}
				textures.swap(std::vector<Texture*>());
			}

			GLuint diffuseMap       = 0;
			GLuint specularMap      = 0;
			GLuint ambientMap       = 0;
			GLuint emissiveMap      = 0;
			GLuint normalMap        = 0;
			GLuint heightMap        = 0;
			GLuint shininessMap     = 0;
			GLuint opacityMap       = 0;
			GLuint displacementMap  = 0;
			GLuint lightMap         = 0;
			GLuint reflectionMap    = 0;
			GLuint unknownMap       = 0;

			GLuint count = 0;

			for (auto& texturePath : texturePaths)
			{
				GLboolean skip = false;
				for (auto& loadedTexture : loadedTextures)
				{
					if (std::strcmp(texturePath.second.c_str(), loadedTexture->getFilePath().c_str()) == 0)
					{
						textures.push_back(loadedTexture);
						skip = true;
						break;
					}
				}

				if (!skip)
				{
					struct stat info;
					if (stat(texturePath.second.c_str(), &info) != 0)
					{
						std::cout << "failed to locate texture file in " + texturePath.second << std::endl;
						return false;
					}

					Texture* texture = Texture::createWithFile(texturePath.second, texturePath.first);
					textures.push_back(texture);
					loadedTextures.insert(texture);
				}

				std::string number;
				std::string name;
				if (texturePath.first == Texture::Type::DIFFUSE)
				{
					name = "diffuseMap_";
					number = std::to_string(diffuseMap++);
				}
				else if (texturePath.first == Texture::Type::SPECULAR)
				{
					name = "specularMap_";
					number = std::to_string(specularMap++);
				}
				else if (texturePath.first == Texture::Type::AMBIENT)
				{
					name = "ambientMap_";
					number = std::to_string(ambientMap++);
				}
				else if (texturePath.first == Texture::Type::EMISSIVE)
				{
					name = "emissiveMap_";
					number = std::to_string(emissiveMap++);
				}
				else if (texturePath.first == Texture::Type::NORMAL)
				{
					name = "normalMap_";
					number = std::to_string(normalMap++);
				}
				else if (texturePath.first == Texture::Type::HEIGHT)
				{
					name = "heightMap_";
					number = std::to_string(heightMap++);
				}
				else if (texturePath.first == Texture::Type::SHININESS)
				{
					name = "shininessMap_";
					number = std::to_string(shininessMap++);
				}
				else if (texturePath.first == Texture::Type::OPACITY)
				{
					name = "opacityMap_";
					number = std::to_string(opacityMap++);
				}
				else if (texturePath.first == Texture::Type::DISPLACEMENT)
				{
					name = "displacementMap_";
					number = std::to_string(displacementMap++);
				}
				else if (texturePath.first == Texture::Type::LIGHTMAP)
				{
					name = "lightMap_";
					number = std::to_string(lightMap++);
				}
				else if (texturePath.first == Texture::Type::REFLECTION)
				{
					name = "reflectionMap_";
					number = std::to_string(reflectionMap++);
				}
				else if (texturePath.first == Texture::Type::UNKNOWN)
				{
					name = "unknownMap_";
					number = std::to_string(unknownMap++);
				}

				setInteger((name + number).c_str(), count++);
			}
			return true;
		}

		bool compileWithFile(Material::ShaderType type, const std::string& filePath, GLuint* shaderID)
		{
			struct stat info;
			if (stat(filePath.c_str(), &info) != 0)
			{
				std::cout << "failed to locate shader file in " + filePath << std::endl;
				return false;
			}

			std::ifstream shaderFile;
			shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

			std::stringstream shaderStream;
			try {
				shaderFile.open(filePath, std::ios::in | std::ios::binary);

				shaderStream << shaderFile.rdbuf();

				shaderFile.close();
			}
			catch (std::ifstream::failure e)
			{
				std::cout << "failed to read shader file in " + filePath << std::endl;
			}

			return compileWithSource(type, shaderStream.str().c_str(), shaderID);
		}

		bool compileWithSource(Material::ShaderType type, const GLchar* source, GLuint* shaderID)
		{
			if (type == Material::ShaderType::VERTEX)
			{
				*shaderID = glCreateShader(GL_VERTEX_SHADER);
			}
			else if (type == Material::ShaderType::TESSELLATION_CONTROL)
			{
				*shaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
			}
			else if (type == Material::ShaderType::TESSELLATION_EVALUATION)
			{
				*shaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);
			}
			else if (type == Material::ShaderType::GEOMETRY)
			{
				*shaderID = glCreateShader(GL_GEOMETRY_SHADER);
			}
			else if (type == Material::ShaderType::FRAGMENT)
			{
				*shaderID = glCreateShader(GL_FRAGMENT_SHADER);
			}
			glShaderSource(*shaderID, 1, &source, nullptr);
			glCompileShader(*shaderID);
			if (!checkCompiled(*shaderID, type))
			{
				return false;
			}
			return true;
		}

		bool checkCompiled(GLuint shader, ShaderType type)
		{
			GLint success;
			GLchar infoLog[1024];
			if (type == ShaderType::VERTEX || type == ShaderType::TESSELLATION_CONTROL || type == ShaderType::TESSELLATION_EVALUATION || type == ShaderType::GEOMETRY || type == ShaderType::FRAGMENT)
			{
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
					std::cout << "shader compilation error : " << infoLog << std::endl;
					return false;
				}
				return true;
			}
			else if (type == ShaderType::PROGRAM)
			{
				glGetProgramiv(shader, GL_LINK_STATUS, &success);
				if (!success)
				{
					glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
					std::cout << "program link error : " << infoLog << std::endl;
					return false;
				}
				isLinkSucceed = true;
				return true;
			}

			std::cout << "unsupported shader type" << std::endl;
			return false;
		}
	};
	std::set<Texture*> Material::loadedTextures = {};
}