#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <ogles.h>
#include <shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <unordered_map>
#include <memory>

namespace es
{
	class Program
	{
	public:
		Program(const std::vector<Shader*>& shaders);
		Program(const std::vector<std::string>& files);
		~Program();

		static std::unique_ptr<Program> createFromShaders(const std::vector<Shader*>& shaders);

		static std::unique_ptr<Program> createFromFiles(const std::vector<std::string>& files);

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

		void initFromShaders(const std::vector<Shader*>& shaders);
	protected:
		//Program(const Program&) = delete;
		//const Program& operator=(const Program&) = delete;
	private:
		GLuint mID;
		std::unordered_map<std::string, GLuint> mLocationMap;
	};
}
#endif