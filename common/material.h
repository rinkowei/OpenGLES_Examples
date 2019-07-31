#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

namespace es
{
	class Material
	{
	public:
		Material() = default;
		~Material()
		{
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

		enum class ShaderType
		{
			Vertex,
			Geometry,
			Fragment,
			Program
		};

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
			if (programID != 0 && isLinkSucceed)
			{
				glUseProgram(programID);
			}
		}

		void setBool(const std::string& name, bool value) const
		{
			glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
		}

		void setInt(const std::string& name, int value) const
		{
			glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
		}

		void setFloat(const std::string& name, float value) const
		{
			glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
		}

		void setVec2(const std::string& name, const glm::vec2& value) const
		{
			glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
		}
		void setVec2(const std::string& name, float x, float y) const
		{
			glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y);
		}

		void setVec3(const std::string& name, const glm::vec3& value) const
		{
			glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
		}
		void setVec3(const std::string& name, float x, float y, float z) const
		{
			glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
		}

		void setVec4(const std::string& name, const glm::vec4& value) const
		{
			glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
		}
		void setVec4(const std::string& name, float x, float y, float z, float w)
		{
			glUniform4f(glGetUniformLocation(programID, name.c_str()), x, y, z, w);
		}

		void setMat2(const std::string& name, const glm::mat2& mat) const
		{
			glUniformMatrix2fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

		void setMat3(const std::string& name, const glm::mat3& mat) const
		{
			glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

		void setMat4(const std::string& name, const glm::mat4& mat) const
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
			auto& shader = shaderPaths.find(Material::ShaderType::Vertex);
			if (shader != shaderPaths.end())
			{
				if (!compileWithFile(Material::ShaderType::Vertex, shader->second, &vertShaderID))
				{
					return false;
				}
				glAttachShader(programID, vertShaderID);
			}

			GLuint geomShaderID = 0;
			shader = shaderPaths.find(Material::ShaderType::Geometry);
			if (shader != shaderPaths.end())
			{
				if (!compileWithFile(Material::ShaderType::Geometry, shader->second, &geomShaderID))
				{
					return false;
				}
				glAttachShader(programID, geomShaderID);
			}

			GLuint fragShaderID = 0;
			shader = shaderPaths.find(Material::ShaderType::Fragment);
			if (shader != shaderPaths.end())
			{
				if (!compileWithFile(Material::ShaderType::Fragment, shader->second, &fragShaderID))
				{
					return false;
				}
				glAttachShader(programID, fragShaderID);
			}

			glLinkProgram(programID);
			if (!checkCompiled(programID, Material::ShaderType::Program))
			{
				return false;
			}

			glDeleteShader(vertShaderID);
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

			GLuint diffuseMap = 0;
			GLuint specularMap = 0;
			GLuint normalMap = 0;
			GLuint heightMap = 0;

			GLuint count = 0;
			textures.resize(texturePaths.size());
			for (auto& texturePath : texturePaths)
			{
				struct stat info;
				if (stat(texturePath.second.c_str(), &info) != 0)
				{
					std::cout << "failed to locate texture file in " + texturePath.second << std::endl;
					return false;
				}

				Texture* texture = Texture::createWithFile(texturePath.second, texturePath.first);
				std::string number;
				std::string name;
				if (texturePath.first == Texture::Type::Diffuse)
				{
					name = "diffuseMap_";
					number = std::to_string(diffuseMap++);
				}
				else if (texturePath.first == Texture::Type::Diffuse)
				{
					name = "specularMap_";
					number = std::to_string(specularMap++);
				}
				else if (texturePath.first == Texture::Type::Diffuse)
				{
					name = "normalMap_";
					number = std::to_string(normalMap++);
				}
				else if (texturePath.first == Texture::Type::Diffuse)
				{
					name = "heightMap_";
					number = std::to_string(heightMap++);
				}

				glActiveTexture(GL_TEXTURE0 + count);
				glBindTexture(GL_TEXTURE_2D, texture->getID());
				this->setInt((name + number).c_str(), count++);

				textures.push_back(texture);
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
			if (type == Material::ShaderType::Vertex)
			{
				*shaderID = glCreateShader(GL_VERTEX_SHADER);
			}
			else if (type == Material::ShaderType::Geometry)
			{
				*shaderID = glCreateShader(GL_GEOMETRY_SHADER);
			}
			else if (type == Material::ShaderType::Fragment)
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
			if (type == ShaderType::Vertex || type == ShaderType::Geometry || type == ShaderType::Fragment)
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
			else if (type == ShaderType::Program)
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
}