#ifndef SHADER_H_
#define SHADER_H_

#include "ogles.h"

#include <vector>
#include <string>

namespace es
{
	class Shader
	{
	public:
		Shader(GLenum type, const std::string& path);
		virtual ~Shader();

		bool isCompiled();
		GLuint getID();
		GLenum getType();
	private:
		bool mCompiled;
		GLuint mID;
		GLenum mType;
	};

	class VertexShader : public Shader
	{
	public:
		VertexShader(const std::string& path);
		~VertexShader();

		static VertexShader* createFromFile(const std::string& path);
	};

	class GeometryShader : public Shader
	{
	public:
		GeometryShader(const std::string& path);
		~GeometryShader();

		static GeometryShader* createFromFile(const std::string& path);
	};

	class FragmentShader : public Shader
	{
	public:
		FragmentShader(const std::string& path);
		~FragmentShader();

		static FragmentShader* createFromFile(const std::string& path);
	};

	class ComputeShader : public Shader
	{
	public:
		ComputeShader(const std::string& path);
		~ComputeShader();

		static ComputeShader* createFromFile(const std::string& path);
	};
}

#endif