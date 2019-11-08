#include "shader.h"
#include "utility.h"

namespace es
{
	Shader::Shader(GLenum type, const std::string& path)
		:mID(0),
		 mCompiled(false),
		 mType(GL_INVALID_ENUM)
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

	VertexShader::VertexShader(const std::string& path) : Shader(GL_VERTEX_SHADER, path)
	{

	}

	VertexShader::~VertexShader()
	{

	}

	VertexShader* VertexShader::createFromFile(const std::string& path)
	{
		VertexShader* shader = new (std::nothrow) VertexShader(path);
		if (shader)
		{
			return shader;
		}
		delete(shader);
		return nullptr;
	}

	GeometryShader::GeometryShader(const std::string& path) : Shader(GL_GEOMETRY_SHADER_EXT, path)
	{

	}

	GeometryShader::~GeometryShader()
	{

	}

	GeometryShader* GeometryShader::createFromFile(const std::string& path)
	{
		GeometryShader* shader = new (std::nothrow) GeometryShader(path);
		if (shader)
		{
			return shader;
		}
		delete(shader);
		return nullptr;
	}

	FragmentShader::FragmentShader(const std::string& path) : Shader(GL_FRAGMENT_SHADER, path)
	{

	}

	FragmentShader::~FragmentShader()
	{

	}

	FragmentShader* FragmentShader::createFromFile(const std::string& path)
	{
		FragmentShader* shader = new (std::nothrow) FragmentShader(path);
		if (shader)
		{
			return shader;
		}
		delete(shader);
		return nullptr;
	}

	ComputeShader::ComputeShader(const std::string& path) : Shader(GL_COMPUTE_SHADER, path)
	{

	}

	ComputeShader::~ComputeShader()
	{

	}

	ComputeShader* ComputeShader::createFromFile(const std::string& path)
	{
		ComputeShader* shader = new (std::nothrow) ComputeShader(path);
		if (shader)
		{
			return shader;
		}
		delete(shader);
		return nullptr;
	}
}
