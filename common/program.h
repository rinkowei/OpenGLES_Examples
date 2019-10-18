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
		Program(const std::string& name, const std::vector<Shader*>& shaders);
		Program(const std::string& name, const std::vector<std::string>& files);
		~Program();

		static std::shared_ptr<Program> createFromShaders(const std::string& name, const std::vector<Shader*>& shaders);

		static std::shared_ptr<Program> createFromFiles(const std::string& name, const std::vector<std::string>& files);

		void apply();
		void unapply();

		void uniformBlockBinding(std::string name, int binding);
		bool setUniform(const std::string& name, const int& value);
		bool setUniform(const std::string& name, const bool& value);
		bool setUniform(const std::string& name, const float& value);
		bool setUniform(const std::string& name, const glm::vec2& value);
		bool setUniform(const std::string& name, const glm::vec3& value);
		bool setUniform(const std::string& name, const glm::vec4& value);
		bool setUniform(const std::string& name, const glm::mat2& value);
		bool setUniform(const std::string& name, const glm::mat3& value);
		bool setUniform(const std::string& name, const glm::mat4& value);
		bool setUniform(const std::string& name, const std::vector<int>& value);
		bool setUniform(const std::string& name, const std::vector<float>& value);
		bool setUniform(const std::string& name, const std::vector<glm::vec2>& value);
		bool setUniform(const std::string& name, const std::vector<glm::vec3>& value);
		bool setUniform(const std::string& name, const std::vector<glm::vec4>& value);
		bool setUniform(const std::string& name, const std::vector<glm::mat2>& value);
		bool setUniform(const std::string& name, const std::vector<glm::mat3>& value);
		bool setUniform(const std::string& name, const std::vector<glm::mat4>& value);

		const std::unordered_map<std::string, GLuint>& getAttribLocationMap() const;
		const std::unordered_map<std::string, GLuint>& getUniformLocationMap() const;
	private:
		void initFromShaders(const std::vector<Shader*>& shaders);

		GLuint mID;
		std::string mName;

		std::unordered_map<std::string, GLuint> mAttribLocationMap;
		std::unordered_map<std::string, GLuint> mUniformLocationMap;

		static std::unordered_map<std::string, std::shared_ptr<Program>> mProgramCache;
	};
}
#endif