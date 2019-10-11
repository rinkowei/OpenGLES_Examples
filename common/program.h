#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <ogles.h>
#include <shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <unordered_map>

namespace es
{
	class Program
	{
	public:
		static Program* createFromShaders(const std::vector<Shader*>& shaders);

		void apply();

		void uniformBlockBinding(std::string name, int binding);
		bool setUniform(const std::string& name, int value);
		bool setUniform(const std::string& name, bool value);
		bool setUniform(const std::string& name, float value);
		bool setUniform(const std::string& name, const glm::vec2& value);
		bool setUniform(const std::string& name, const glm::vec3& value);
		bool setUniform(const std::string& name, const glm::vec4& value);
		bool setUniform(const std::string& name, const glm::mat2& value);
		bool setUniform(const std::string& name, const glm::mat3& value);
		bool setUniform(const std::string& name, const glm::mat4& value);
		bool setUniform(const std::string& name, int count, int* value);
		bool setUniform(const std::string& name, int count, float* value);
		bool setUniform(const std::string& name, int count, glm::vec2* value);
		bool setUniform(const std::string& name, int count, glm::vec3* value);
		bool setUniform(const std::string& name, int count, glm::vec4* value);
		bool setUniform(const std::string& name, int count, glm::mat2* value);
		bool setUniform(const std::string& name, int count, glm::mat3* value);
		bool setUniform(const std::string& name, int count, glm::mat4* value);
	private:
		Program(const std::vector<Shader*>& shaders);
		~Program();

		GLuint mID;
		std::unordered_map<std::string, GLuint> mLocationMap;
	};
}

#endif