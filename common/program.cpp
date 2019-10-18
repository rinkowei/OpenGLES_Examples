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
		mUniformLocationMap.swap(std::unordered_map<std::string, GLuint>());
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

	void Program::unapply()
	{
		GLES_CHECK_ERROR(glUseProgram(0));
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

	bool Program::setUniform(const std::string& name, const int& value)
	{
		if (mUniformLocationMap.find(name) == mUniformLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glProgramUniform1i(mID, mUniformLocationMap[name], value));

		return true;
	}

	bool Program::setUniform(const std::string& name, bool value)
	{
		if (mUniformLocationMap.find(name) == mUniformLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glProgramUniform1i(mID, mUniformLocationMap[name], (int)value));

		return true;
	}

	bool Program::setUniform(const std::string& name, const float& value)
	{
		if (mUniformLocationMap.find(name) == mUniformLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glProgramUniform1f(mID, mUniformLocationMap[name], value));

		return true;
	}

	bool Program::setUniform(const std::string& name, const glm::vec2& value)
	{
		if (mUniformLocationMap.find(name) == mUniformLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glProgramUniform2f(mID, mUniformLocationMap[name], value.x, value.y));

		return true;
	}

	bool Program::setUniform(const std::string& name, const glm::vec3& value)
	{
		if (mUniformLocationMap.find(name) == mUniformLocationMap.end())
		{ 
			return false;
		}

		GLES_CHECK_ERROR(glProgramUniform3f(mID, mUniformLocationMap[name], value.x, value.y, value.z));

		return true;
	}

	bool Program::setUniform(const std::string& name, const glm::vec4& value)
	{
		if (mUniformLocationMap.find(name) == mUniformLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glProgramUniform4f(mID, mUniformLocationMap[name], value.x, value.y, value.z, value.w));

		return true;
	}

	bool Program::setUniform(const std::string& name, const glm::mat2& value)
	{
		if (mUniformLocationMap.find(name) == mUniformLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glProgramUniformMatrix2fv(mID, mUniformLocationMap[name], 1, GL_FALSE, glm::value_ptr(value)));

		return true;
	}

	bool Program::setUniform(const std::string& name, const glm::mat3& value)
	{
		if (mUniformLocationMap.find(name) == mUniformLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glProgramUniformMatrix3fv(mID, mUniformLocationMap[name], 1, GL_FALSE, glm::value_ptr(value)));

		return true;
	}

	bool Program::setUniform(const std::string& name, const glm::mat4& value)
	{
		if (mUniformLocationMap.find(name) == mUniformLocationMap.end())
		{
			return false;
		}
		
		GLES_CHECK_ERROR(glProgramUniformMatrix4fv(mID, mUniformLocationMap[name], 1, GL_FALSE, glm::value_ptr(value)));
	
		return true;
	}

	bool Program::setUniform(const std::string& name, const std::vector<int>& value)
	{
		if (mUniformLocationMap.find(name) == mUniformLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glProgramUniform1iv(mID, mUniformLocationMap[name], value.size(), value.data()));

		return true;
	}

	bool Program::setUniform(const std::string& name, const std::vector<float>& value)
	{
		if (mUniformLocationMap.find(name) == mUniformLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glProgramUniform1fv(mID, mUniformLocationMap[name], value.size(), value.data()));

		return true;
	}

	bool Program::setUniform(const std::string& name, const std::vector<glm::vec2>& value)
	{
		if (mUniformLocationMap.find(name) == mUniformLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glProgramUniform2fv(mID, mUniformLocationMap[name], value.size(), glm::value_ptr(value[0])));

		return true;
	}

	bool Program::setUniform(const std::string& name, const std::vector<glm::vec3>& value)
	{
		if (mUniformLocationMap.find(name) == mUniformLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glProgramUniform3fv(mID, mUniformLocationMap[name], value.size(), glm::value_ptr(value[0])));

		return true;
	}

	bool Program::setUniform(const std::string& name, const std::vector<glm::vec4>& value)
	{
		if (mUniformLocationMap.find(name) == mUniformLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glProgramUniform4fv(mID, mUniformLocationMap[name], value.size(), glm::value_ptr(value[0])));

		return true;
	}

	bool Program::setUniform(const std::string& name, const std::vector<glm::mat2>& value)
	{
		if (mUniformLocationMap.find(name) == mUniformLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glProgramUniformMatrix2fv(mID, mUniformLocationMap[name], value.size(), GL_FALSE, glm::value_ptr(value[0])));

		return true;
	}

	bool Program::setUniform(const std::string& name, const std::vector<glm::mat3>& value)
	{
		if (mUniformLocationMap.find(name) == mUniformLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glProgramUniformMatrix3fv(mID, mUniformLocationMap[name], value.size(), GL_FALSE, glm::value_ptr(value[0])));

		return true;
	}

	bool Program::setUniform(const std::string& name, const std::vector<glm::mat4>& value)
	{
		if (mUniformLocationMap.find(name) == mUniformLocationMap.end())
		{
			return false;
		}

		GLES_CHECK_ERROR(glProgramUniformMatrix4fv(mID, mUniformLocationMap[name], value.size(), GL_FALSE, glm::value_ptr(value[0])));

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

		GLint size;
		GLenum type;
		GLsizei length;
		const GLuint bufSize = 64;
		GLchar name[bufSize];

		int attribCount = 0;
		GLES_CHECK_ERROR(glGetProgramiv(mID, GL_ACTIVE_ATTRIBUTES, &attribCount));
		for (int i = 0; i < attribCount; i++)
		{
			GLES_CHECK_ERROR(glGetActiveAttrib(mID, i, bufSize, &length, &size, &type, name));
			GLES_CHECK_ERROR(GLuint loc = glGetAttribLocation(mID, name));

			if (loc != GL_INVALID_INDEX)
			{
				mAttribLocationMap[std::string(name)] = loc;
			}
		}

		int uniformCount = 0;
		GLES_CHECK_ERROR(glGetProgramiv(mID, GL_ACTIVE_UNIFORMS, &uniformCount));
		for (int i = 0; i < uniformCount; i++)
		{
			GLES_CHECK_ERROR(glGetActiveUniform(mID, i, bufSize, &length, &size, &type, name));
			GLES_CHECK_ERROR(GLuint loc = glGetUniformLocation(mID, name));

			if (loc != GL_INVALID_INDEX)
			{
				mUniformLocationMap[std::string(name)] = loc;
			}
		}
	}

	const std::unordered_map<std::string, GLuint>& Program::getAttribLocationMap() const
	{
		return mAttribLocationMap;
	}

	const std::unordered_map<std::string, GLuint>& Program::getUniformLocationMap() const
	{
		return mUniformLocationMap;
	}
}
