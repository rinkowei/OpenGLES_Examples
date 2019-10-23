#include "shader.h"
#include "utility.h"

namespace es
{
	Shader::Shader(GLenum type, const std::string& path)
		:mID(0),
		 mCompiled(false),
		 mType(GL_VERTEX_SHADER)
	{
		std::string shaderStr;
		if (!Utility::readFile(path, shaderStr))
		{
			return;
		}

		mType = type;
		GLES_CHECK_ERROR(mID = glCreateShader(type));

		GLint success;
		GLchar log[512];

		const GLchar* source = shaderStr.c_str();
		GLES_CHECK_ERROR(glShaderSource(mID, 1, &source, nullptr));
		GLES_CHECK_ERROR(glCompileShader(mID));
		GLES_CHECK_ERROR(glGetShaderiv(mID, GL_COMPILE_STATUS, &success));

		if (success == GL_FALSE)
		{
			GLES_CHECK_ERROR(glGetShaderInfoLog(mID, 512, nullptr, log));

			std::string logError = "OpenGL ES : shader compilation failed: ";
			logError += std::string(log);

			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, logError.c_str());
			mCompiled = false;
		}
		else
		{
			mCompiled = true;
		}
	}

	Shader::~Shader()
	{
		GLES_CHECK_ERROR(glDeleteShader(mID));
	}

	Shader* Shader::createFromFile(GLenum type, const std::string& path)
	{
		Shader* shader = new (std::nothrow) Shader(type, path);
		if (shader)
		{
			return shader;
		}
		delete(shader);
		return nullptr;
	}

	bool Shader::isCompiled()
	{
		return mCompiled;
	}

	GLuint Shader::getID()
	{
		return mID;
	}

	GLenum Shader::getType()
	{
		return mType;
	}
}
