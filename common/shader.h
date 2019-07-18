#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	Shader() {};
	~Shader() {};

	enum class TYPE {
		VERTEX,
		FRAGMENT,
		PROGRAM
	};

	static Shader* createWithFile(const std::string& vsFile, const std::string& fsFile)
	{
		Shader* shader = new (std::nothrow) Shader();
		if (shader && shader->initWithFile(vsFile, fsFile))
		{
			return shader;
		}
		delete(shader);
		return nullptr;
	}

	static Shader* createWithSource(const std::string& vsSource, const std::string& fsSource)
	{
		Shader* shader = new (std::nothrow) Shader();
		if (shader && shader->initWithSource(vsSource, fsSource))
		{
			return shader;
		}
		delete(shader);
		return nullptr;
	}

	Shader(const std::string& vsPath, const std::string& fsPath)
	{
		std::string vsCode;
		std::string fsCode;
		std::ifstream vsShaderFile;
		std::ifstream fsShaderFile;
		
		vsShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fsShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		
		try {
			vsShaderFile.open(vsPath, std::ios::in | std::ios::binary);
			fsShaderFile.open(fsPath, std::ios::in | std::ios::binary);

			std::stringstream vsShaderStream;
			std::stringstream fsShaderStream;

			vsShaderStream << vsShaderFile.rdbuf();
			fsShaderStream << fsShaderFile.rdbuf();
		
			vsShaderFile.close();
			fsShaderFile.close();

			vsCode = vsShaderStream.str();
			fsCode = fsShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR : shader file failed to read" << std::endl;
		}

		GLuint vsShaderID;
		GLuint fsShaderID;

		const GLchar* vsShaderCode = vsCode.c_str();
		const GLchar* fsShaderCode = fsCode.c_str();

		vsShaderID = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vsShaderID, 1, &vsShaderCode, nullptr);
		glCompileShader(vsShaderID);
		checkCompileErrors(vsShaderID, Shader::TYPE::VERTEX);

		fsShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fsShaderID, 1, &fsShaderCode, nullptr);
		glCompileShader(fsShaderID);
		checkCompileErrors(fsShaderID, Shader::TYPE::FRAGMENT);

		ID = glCreateProgram();
		glAttachShader(ID, vsShaderID);
		glAttachShader(ID, fsShaderID);
		glLinkProgram(ID);
		checkCompileErrors(ID, Shader::TYPE::PROGRAM);

		glDeleteShader(vsShaderID);
		glDeleteShader(fsShaderID);
	}

	void use()
	{
		glUseProgram(ID);
	}

	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setVec2(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec2(const std::string& name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}

	void setVec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}

	void setVec4(const std::string& name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec4(const std::string& name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}

	void setMat2(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat3(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat4(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	GLuint getID()
	{
		return ID;
	}
private:
	GLuint ID = 0;

	bool initWithFile(const std::string& vsFile, const std::string& fsFile)
	{
		struct stat info;
		if (stat(vsFile.c_str(), &info) != 0)
		{
			std::runtime_error("failed to locate vertex shader file in " + vsFile);
			return false;
		}
		if (stat(fsFile.c_str(), &info) != 0)
		{
			std::runtime_error("failed to locate fragment shader file in " + fsFile);
			return false;
		}

		std::string vsCode;
		std::string fsCode;
		std::ifstream vsShaderFile;
		std::ifstream fsShaderFile;

		vsShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fsShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			vsShaderFile.open(vsFile, std::ios::in | std::ios::binary);
			fsShaderFile.open(fsFile, std::ios::in | std::ios::binary);

			std::stringstream vsShaderStream;
			std::stringstream fsShaderStream;

			vsShaderStream << vsShaderFile.rdbuf();
			fsShaderStream << fsShaderFile.rdbuf();

			vsShaderFile.close();
			fsShaderFile.close();

			vsCode = vsShaderStream.str();
			fsCode = fsShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR : shader file failed to read" << std::endl;
		}

		return initWithSource(vsCode, fsCode);
	}

	bool initWithSource(const std::string& vsSource, const std::string& fsSource)
	{
		GLuint vsShaderID;
		GLuint fsShaderID;

		const GLchar* vsShaderCode = vsSource.c_str();
		const GLchar* fsShaderCode = fsSource.c_str();

		vsShaderID = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vsShaderID, 1, &vsShaderCode, nullptr);
		glCompileShader(vsShaderID);
		if (!checkCompileErrors(vsShaderID, Shader::TYPE::VERTEX))
			return false;

		fsShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fsShaderID, 1, &fsShaderCode, nullptr);
		glCompileShader(fsShaderID);
		if (!checkCompileErrors(fsShaderID, Shader::TYPE::FRAGMENT))
			return false;

		ID = glCreateProgram();
		glAttachShader(ID, vsShaderID);
		glAttachShader(ID, fsShaderID);
		glLinkProgram(ID);
		if (!checkCompileErrors(ID, Shader::TYPE::PROGRAM))
			return false;

		glDeleteShader(vsShaderID);
		glDeleteShader(fsShaderID);

		return true;
	}

	bool checkCompileErrors(GLuint shader, Shader::TYPE type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type == Shader::TYPE::VERTEX || type == Shader::TYPE::FRAGMENT)
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
				std::cout << "shader compilation error : \n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
				return false;
			}
			return true;
		}
		else if (type == Shader::TYPE::PROGRAM)
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
				std::cout << "program link error : " << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
				return false;
			}
			return true;
		}
		return false;
	}
};