#include "program.h"

namespace es
{
	Program::Program()
	{
		mID = 0;
	}

	Program::~Program()
	{
		GLES_CHECK_ERROR(glDeleteProgram(mID));
	}

	Program* Program::createFromShaders(const std::vector<Shader*>& shaders)
	{
		Program* program = new (std::nothrow) Program();
		if (program && program->initFromShaders(shaders))
		{
			return program;
		}
		delete(program);
		return nullptr;
	}

	void Program::apply()
	{
		GLES_CHECK_ERROR(glUseProgram(mID));
	}

	void Program::uniformBlockBinding(std::string name, int binding)
	{

	}

	bool Program::setUniform(std::string name, int value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		glUniform1i(mLocationMap[name], value);

		return true;
	}

	bool Program::setUniform(std::string name, float value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		glUniform1f(mLocationMap[name], value);

		return true;
	}

	bool Program::setUniform(std::string name, glm::vec2 value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		glUniform2f(mLocationMap[name], value.x, value.y);

		return true;
	}

	bool Program::setUniform(std::string name, glm::vec3 value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		glUniform3f(mLocationMap[name], value.x, value.y, value.z);

		return true;
	}

	bool Program::setUniform(std::string name, glm::vec4 value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		glUniform4f(mLocationMap[name], value.x, value.y, value.z, value.w);

		return true;
	}

	bool Program::setUniform(std::string name, glm::mat2 value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		glUniformMatrix2fv(mLocationMap[name], 1, GL_FALSE, glm::value_ptr(value));

		return true;
	}

	bool Program::setUniform(std::string name, glm::mat3 value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		glUniformMatrix3fv(mLocationMap[name], 1, GL_FALSE, glm::value_ptr(value));

		return true;
	}

	bool Program::setUniform(std::string name, glm::mat4 value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		glUniformMatrix4fv(mLocationMap[name], 1, GL_FALSE, glm::value_ptr(value));

		return true;
	}

	bool Program::setUniform(std::string name, int count, int* value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		glUniform1iv(mLocationMap[name], count, value);

		return true;
	}

	bool Program::setUniform(std::string name, int count, float* value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		glUniform1fv(mLocationMap[name], count, value);

		return true;
	}

	bool Program::setUniform(std::string name, int count, glm::vec2* value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		glUniform2fv(mLocationMap[name], count, glm::value_ptr(value[0]));

		return true;
	}

	bool Program::setUniform(std::string name, int count, glm::vec3* value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		glUniform3fv(mLocationMap[name], count, glm::value_ptr(value[0]));

		return true;
	}

	bool Program::setUniform(std::string name, int count, glm::vec4* value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		glUniform4fv(mLocationMap[name], count, glm::value_ptr(value[0]));

		return true;
	}

	bool Program::setUniform(std::string name, int count, glm::mat2* value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		glUniformMatrix2fv(mLocationMap[name], count, GL_FALSE, glm::value_ptr(value[0]));

		return true;
	}

	bool Program::setUniform(std::string name, int count, glm::mat3* value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		glUniformMatrix3fv(mLocationMap[name], count, GL_FALSE, glm::value_ptr(value[0]));

		return true;
	}

	bool Program::setUniform(std::string name, int count, glm::mat4* value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		glUniformMatrix4fv(mLocationMap[name], count, GL_FALSE, glm::value_ptr(value[0]));

		return true;
	}

	bool Program::initFromShaders(const std::vector<Shader*>& shaders)
	{
		GLES_CHECK_ERROR(mID = glCreateProgram());
		for (std::size_t i = 0; i < shaders.size(); i++)
		{
			GLES_CHECK_ERROR(glAttachShader(mID, shaders[i]->getID()));
		}

		GLES_CHECK_ERROR(glLinkProgram(mID));

		GLint success;
		char log[512];

		GLES_CHECK_ERROR(glGetProgramiv(mID, GL_LINK_STATUS, &success));

		if (!success)
		{
			glGetProgramInfoLog(mID, 512, nullptr, log);

			std::string logError = "OpenGL ES : failed to link shader program : ";
			logError += std::string(log);

			SDL_LogError(SDL_LOG_CATEGORY_ERROR, logError.c_str());

			return false;
		}

		int uniformCount = 0;
		GLES_CHECK_ERROR(glGetProgramiv(mID, GL_ACTIVE_UNIFORMS, &uniformCount));

		GLint size;
		GLenum type;
		GLsizei length;
		const GLuint bufSize = 64;
		GLchar name[bufSize];

		for (int i = 0; i < uniformCount; i++)
		{
			GLES_CHECK_ERROR(glGetActiveUniform(mID, i, bufSize, &length, &size, &type, name));
			GLES_CHECK_ERROR(GLuint loc = glGetUniformLocation(mID, name));

			if (loc != GL_INVALID_INDEX)
			{
				mLocationMap[std::string(name)] = loc;
			}
		}
	}
}
