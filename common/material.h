#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

class Material
{
public:
	Material() = default;
	~Material() = default;

	enum class ShaderType
	{
		Vertex,
		Geometry,
		Fragment,
		Program
	};

	static Material* createWithFile(const std::unordered_map<Material::ShaderType, std::string>& shaderPaths)
	{
		Material* material = new (std::nothrow) Material();
		if (material && material->initWithFile(shaderPaths))
		{
			return material;
		}
		delete(material);
		return nullptr;
	}

	void use()
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

	bool initWithFile(const std::unordered_map<Material::ShaderType, std::string>& shaderPaths)
	{
		return true;
	}

	GLuint compileWithFile(Material::ShaderType type, const std::string& filePath)
	{
		GLuint shaderID = 0;
		if (type == Material::ShaderType::Vertex)
		{

		}
	}

	GLuint compileWithSource(Material::ShaderType type, const GLchar* source)
	{
		GLuint shaderID = 0;

		if (type == Material::ShaderType::Vertex)
		{
			shaderID = glCreateShader(GL_VERTEX_SHADER);
		}
		else if (type == Material::ShaderType::Geometry)
		{
			shaderID = glCreateShader(GL_GEOMETRY_SHADER);
		}
		else if (type == Material::ShaderType::Fragment)
		{
			shaderID = glCreateShader(GL_FRAGMENT_SHADER);
		}
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