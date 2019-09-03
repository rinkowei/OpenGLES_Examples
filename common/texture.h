#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <stb/stb_image.h>

using namespace std;

namespace es
{
	class Texture
	{
	public:
		Texture() = default;
		~Texture()
		{
			glDeleteTextures(1, &ID);
		}

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

		static Texture* createWithFile(const std::string& path, Type type)
		{
			Texture* texture = new (std::nothrow) Texture();
			if (texture && texture->initWithFile(path, type))
			{
				return texture;
			}
			delete(texture);
			return nullptr;
		}

		GLuint getID() const
		{
			return ID;
		}

		glm::vec2 getSize() const
		{
			return glm::vec2(width, height);
		}

		GLint getChannelCount() const
		{
			return channelCount;
		}

		GLenum getFormat() const
		{
			return format;
		}

		std::string getFilePath() const
		{
			return filePath;
		}

		Type getType() const
		{
			return type;
		}
	private:
		GLuint ID = 0;
		GLint width = 0;
		GLint height = 0;
		GLint channelCount = 0;
		GLenum format = GL_RGBA;

		std::string filePath;
		Type type = Type::UNKNOWN;

		GLboolean initWithFile(const std::string& path, Type type)
		{
			this->filePath = path;
			this->type = type;

			glGenTextures(1, &ID);
			stbi_set_flip_vertically_on_load(true);
			unsigned char* data = stbi_load(path.c_str(), &width, &height, &channelCount, 0);
			if (data)
			{
				switch (channelCount)
				{
				case 1:
				{
					format = GL_RED;
					break;
				}
				case 3:
				{
					format = GL_RGB;
					break;
				}
				case 4:
				{
					format = GL_RGBA;
					break;
				}
				}

				glBindTexture(GL_TEXTURE_2D, ID);
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				stbi_image_free(data);
			}
			else
			{
				std::cout << "failed to load texture at path: " + path << std::endl;
				stbi_image_free(data);
				return false;
			}

			return true;
		}
	};

	class TextureCube
	{
	public:
		TextureCube() = default;
		~TextureCube()
		{
			glDeleteTextures(1, &ID);
		}

		static TextureCube* createWithFiles(const std::vector<std::string>& filePaths)
		{
			TextureCube* textureCube = new (std::nothrow) TextureCube();
			if (textureCube && textureCube->initWithFiles(filePaths))
			{
				return textureCube;
			}
			delete(textureCube);
			return nullptr;
		}

		GLuint getID() const
		{
			return ID;
		}
	private:
		GLuint ID = 0;

		std::vector<std::string> filePaths;

		GLboolean initWithFiles(const std::vector<std::string>& filePaths)
		{
			this->filePaths = filePaths;

			glGenTextures(1, &ID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

			int width, height, nrChannels;
			for (size_t i = 0; i < filePaths.size(); i++)
			{
				unsigned char* data = stbi_load(filePaths[i].c_str(), &width, &height, &nrChannels, 0);
				if (data)
				{
					if (nrChannels == 1)
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
					else if (nrChannels == 3)
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					else if (nrChannels == 4)
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
					stbi_image_free(data);
				}
				else
				{
					std::cout << "failed to create cubemap texture at path : " << filePaths[i] << std::endl;
					stbi_image_free(data);
				}
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			return true;
		}
	};
}