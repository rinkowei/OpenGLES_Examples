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

	}

	bool Program::setUniform(std::string name, float value)
	{

	}

	bool Program::setUniform(std::string name, glm::vec2 value)
	{

	}

	bool Program::setUniform(std::string name, glm::vec3 value)
	{

	}

	bool Program::setUniform(std::string name, glm::vec4 value)
	{

	}

	bool Program::setUniform(std::string name, glm::mat2 value)
	{

	}

	bool Program::setUniform(std::string name, glm::mat3 value)
	{

	}

	bool Program::setUniform(std::string name, glm::mat4 value)
	{

	}

	bool Program::setUniform(std::string name, int count, int* value)
	{

	}

	bool Program::setUniform(std::string name, int count, float* value)
	{

	}

	bool Program::setUniform(std::string name, int count, glm::vec2* value)
	{

	}

	bool Program::setUniform(std::string name, int count, glm::vec3* value)
	{

	}

	bool Program::setUniform(std::string name, int count, glm::vec4* value)
	{

	}

	bool Program::setUniform(std::string name, int count, glm::mat2* value)
	{

	}

	bool Program::setUniform(std::string name, int count, glm::mat3* value)
	{

	}

	bool Program::setUniform(std::string name, int count, glm::mat4* value)
	{

	}

	bool Program::initFromShaders(const std::vector<Shader*>& shaders)
	{

	}
}
