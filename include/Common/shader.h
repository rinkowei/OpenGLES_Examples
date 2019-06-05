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
	unsigned int ID;

	Shader(const char* vsPath, const char* fsPath)
	{
		std::string vsCode;
		std::string fsCode;
		std::ifstream vsShaderFile;
		std::ifstream fsShaderFile;

		vsShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fsShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			vsShaderFile.open(vsPath);
			fsShaderFile.open(fsPath);
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

		unsigned int vsShaderID;
		unsigned int fsShaderID;

		const char* vsShaderCode = vsCode.c_str();
		const char* fsShaderCode = fsCode.c_str();

		vsShaderID = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vsShaderID, 1, &vsShaderCode, nullptr);
		glCompileShader(vsShaderID);
		checkCompileErrors(vsShaderID, "SHADER");

		fsShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fsShaderID, 1, &fsShaderCode, nullptr);
		glCompileShader(fsShaderID);
		checkCompileErrors(fsShaderID, "SHADER");

		ID = glCreateProgram();
		glAttachShader(ID, vsShaderID);
		glAttachShader(ID, fsShaderID);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");

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
private:
	void checkCompileErrors(GLuint shader, const std::string& type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
				std::cout << "ERROR : shader compilation of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
				std::cout << "ERROR : program link error of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
};