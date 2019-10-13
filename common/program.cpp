#include "program.h"
#include <utility.h>

namespace es
{
	Program::Program(const std::vector<Shader*>& shaders)
		:mID(0)
	{
		initFromShaders(shaders);
	}

	Program::Program(const std::vector<std::string>& files)
		:mID(0)
	{
		std::vector<Shader*> shaders;
		for (std::size_t i = 0; i < files.size(); i++)
		{
			std::string ext = Utility::fileExtension(files[i]);
			GLenum shaderType = GL_VERTEX_SHADER;
			if (ext == "vert")
			{
				shaderType = GL_VERTEX_SHADER;
			}
			else if (ext == "frag")
			{
				shaderType = GL_FRAGMENT_SHADER;
			}

			Shader* shader = Shader::createFromFile(shaderType, files[i]);
			shaders.push_back(shader);
		}

		initFromShaders(shaders);
	}

	Program::~Program()
	{
		mLocationMap.swap(std::unordered_map<std::string, GLuint>());
		GLES_CHECK_ERROR(glDeleteProgram(mID));
	}

	std::unique_ptr<Program> Program::createFromShaders(const std::vector<Shader*>& shaders)
	{
		return std::make_unique<Program>(shaders);
	}

	std::unique_ptr<Program> Program::createFromFiles(const std::vector<std::string>& files)
	{
		return std::make_unique<Program>(files);
	}

	void Program::apply()
	{
		GLES_CHECK_ERROR(glUseProgram(mID));
	}

	void Program::uniformBlockBinding(std::string name, int binding)
	{
		GLES_CHECK_ERROR(GLuint idx = glGetUniformBlockIndex(mID, name.c_str()));

		if (idx == GL_INVALID_INDEX)
		{
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "OPENGL: Failed to get Uniform Block Index for Uniform Buffer : %s", name.c_str());
		}
		else
		{
			GLES_CHECK_ERROR(glUniformBlockBinding(mID, idx, binding));
		}
	}

	bool Program::setUniform(const std::string& name, int value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glUniform1i(mLocationMap[name], value));

		return true;
	}

	bool Program::setUniform(const std::string& name, bool value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glUniform1i(mLocationMap[name], (int)value));

		return true;
	}

	bool Program::setUniform(const std::string& name, float value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glUniform1f(mLocationMap[name], value));

		return true;
	}

	bool Program::setUniform(const std::string& name, const glm::vec2& value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glUniform2f(mLocationMap[name], value.x, value.y));

		return true;
	}

	bool Program::setUniform(const std::string& name, const glm::vec3& value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{ 
			return false;
		}

		GLES_CHECK_ERROR(glUniform3f(mLocationMap[name], value.x, value.y, value.z));

		return true;
	}

	bool Program::setUniform(const std::string& name, const glm::vec4& value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glUniform4f(mLocationMap[name], value.x, value.y, value.z, value.w));

		return true;
	}

	bool Program::setUniform(const std::string& name, const glm::mat2& value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glUniformMatrix2fv(mLocationMap[name], 1, GL_FALSE, glm::value_ptr(value)));

		return true;
	}

	bool Program::setUniform(const std::string& name, const glm::mat3& value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glUniformMatrix3fv(mLocationMap[name], 1, GL_FALSE, glm::value_ptr(value)));

		return true;
	}

	bool Program::setUniform(const std::string& name, const glm::mat4& value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glUniformMatrix4fv(mLocationMap[name], 1, GL_FALSE, glm::value_ptr(value)));

		return true;
	}

	bool Program::setUniform(const std::string& name, int count, int* value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glUniform1iv(mLocationMap[name], count, value));

		return true;
	}

	bool Program::setUniform(const std::string& name, int count, float* value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glUniform1fv(mLocationMap[name], count, value));

		return true;
	}

	bool Program::setUniform(const std::string& name, int count, glm::vec2* value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glUniform2fv(mLocationMap[name], count, glm::value_ptr(value[0])));

		return true;
	}

	bool Program::setUniform(const std::string& name, int count, glm::vec3* value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glUniform3fv(mLocationMap[name], count, glm::value_ptr(value[0])));

		return true;
	}

	bool Program::setUniform(const std::string& name, int count, glm::vec4* value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glUniform4fv(mLocationMap[name], count, glm::value_ptr(value[0])));

		return true;
	}

	bool Program::setUniform(const std::string& name, int count, glm::mat2* value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glUniformMatrix2fv(mLocationMap[name], count, GL_FALSE, glm::value_ptr(value[0])));

		return true;
	}

	bool Program::setUniform(const std::string& name, int count, glm::mat3* value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glUniformMatrix3fv(mLocationMap[name], count, GL_FALSE, glm::value_ptr(value[0])));

		return true;
	}

	bool Program::setUniform(const std::string& name, int count, glm::mat4* value)
	{
		if (mLocationMap.find(name) == mLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glUniformMatrix4fv(mLocationMap[name], count, GL_FALSE, glm::value_ptr(value[0])));

		return true;
	}

	void Program::initFromShaders(const std::vector<Shader*>& shaders)
	{
		GLES_CHECK_ERROR(mID = glCreateProgram());
		for (std::size_t i = 0; i < shaders.size(); i++)
		{
			GLES_CHECK_ERROR(glAttachShader(mID, shaders[i]->getID()));
		}

		GLES_CHECK_ERROR(glLinkProgram(mID));

		for (std::size_t i = 0; i < shaders.size(); i++)
		{
			GLES_CHECK_ERROR(glDeleteShader(shaders[i]->getID()));
		}

		GLint success;
		char log[512];

		GLES_CHECK_ERROR(glGetProgramiv(mID, GL_LINK_STATUS, &success));

		if (!success)
		{
			glGetProgramInfoLog(mID, 512, nullptr, log);

			std::string logError = "OpenGL ES : failed to link shader program : ";
			logError += std::string(log);

			SDL_LogError(SDL_LOG_CATEGORY_ERROR, logError.c_str());

			return;
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
