#pragma once

#include "ogles.h"
#include "program.h"

#include <vector>
#include <string>

namespace es
{
	class Shader
	{
		friend class Program;
	public:
		Shader();
		~Shader();

		static Shader* createFromFile(GLenum type, const std::string& path);

		bool isCompiled();
		GLuint getID();
		GLenum getType();
	private:
		bool initFromFile(GLenum type, const std::string& path);

		bool mCompiled;
		GLuint mID;
		GLenum mType;
	};
}