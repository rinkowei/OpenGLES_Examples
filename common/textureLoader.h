#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <stb/stb_image.h>

using namespace std;

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
		Diffuse,
		Specular,
		Normal,
		Height
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
	Type type = Type::Diffuse;

	bool initWithFile(const std::string& path, Type type)
	{
		this->filePath = path;
		this->type = type;

		glGenTextures(1, &ID);

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
		}
		else
		{
			std::cout << "failed to load texture at path: " + path << std::endl;
			return false;
		}

		return true;
	}
};
class textureLoader
{
public:
	static GLuint loadTexture2D(const string& path, bool isGenerateMipmap)
	{
		GLuint textureID;
		glGenTextures(1, &textureID);

		GLint width, height, nrComponents;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format = GL_RGBA;
			if (nrComponents == 1)
			{
				format = GL_RED;
			}
			else if (nrComponents == 3)
			{
				format = GL_RGB;
			}
			else if (nrComponents == 4)
			{
				format = GL_RGBA;
			}

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			if (isGenerateMipmap)
			{
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			throw std::runtime_error("failed to load texture at path: " + path);
		}

		stbi_image_free(data);

		return textureID;
	}
};