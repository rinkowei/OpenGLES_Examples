#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <stb/stb_image.h>

namespace es
{
	class Texture
	{
	public:
		enum class Type
		{
			DIFFUSE,
			SPECULAR,
			AMBIENT,
			EMISSIVE,
			NORMAL,
			HEIGHT,
			SHININESS,
			OPACITY,
			DISPLACEMENT,
			LIGHTMAP,
			REFLECTION,
			UNKNOWN
		};

		Texture();
		~Texture();

		static Texture* createWithFile(const std::string& path, Type type);

		GLuint getID() const;

		glm::vec2 getSize() const;

		GLint getChannelCount() const;

		GLenum getFormat() const;

		std::string getFilePath() const;

		Texture::Type getType() const;

	private:
		GLuint ID = 0;
		GLint width = 0;
		GLint height = 0;
		GLint channelCount = 0;
		GLenum format = GL_RGBA;

		std::string filePath;
		Type type = Type::UNKNOWN;

		GLboolean initWithFile(const std::string& path, Texture::Type type);
	};

	class TextureCube
	{
	public:
		TextureCube();
		~TextureCube();

		static TextureCube* createWithFiles(const std::vector<std::string>& filePaths);

		GLuint getID() const;
	private:
		GLuint ID = 0;

		std::vector<std::string> filePaths;

		GLboolean initWithFiles(const std::vector<std::string>& filePaths);
	};
}