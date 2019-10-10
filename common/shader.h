#pragma once

#include "ogles.h"

#include <vector>
#include <string>

namespace es
{
	class Shader
	{
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