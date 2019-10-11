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
		static Shader* createFromFile(GLenum type, const std::string& path);

		bool isCompiled();
		GLuint getID();
		GLenum getType();
	private:
		Shader(GLenum type, const std::string& path);
		~Shader();

		bool mCompiled;
		GLuint mID;
		GLenum mType;
	};
}

#endif