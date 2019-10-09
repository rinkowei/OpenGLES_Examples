#include "texture.h"

namespace es
{
	Texture::Texture()
	{
		GLES_CHECK_ERROR(glGenTextures(1, &mID));
	}

	Texture::~Texture()
	{
		GLES_CHECK_ERROR(glDeleteTextures(1, &mID));
	}

	void Texture::bind(uint32_t unit)
	{
		GLES_CHECK_ERROR(glActiveTexture(GL_TEXTURE0 + unit));
		GLES_CHECK_ERROR(glBindTexture(mTarget, mID));
	}

	void Texture::unbind(uint32_t unit)
	{
		GLES_CHECK_ERROR(glActiveTexture(GL_TEXTURE0 + unit));
		GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
	}

	void Texture::generateMipmaps()
	{
		GLES_CHECK_ERROR(glBindTexture(mTarget, mID));
		GLES_CHECK_ERROR(glGenerateMipmap(mTarget));
		GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
	}

	GLuint Texture::getID()
	{
		return mID;
	}

	GLenum Texture::getTarget()
	{
		return mTarget;
	}

	GLenum Texture::getInternalFormat()
	{
		return mInternalFormat;
	}

	GLenum Texture::getFormat()
	{
		return mFormat;
	}

	GLenum Texture::getType()
	{
		return mType;
	}

	uint32_t Texture::getArraySize()
	{
		return mArraySize;
	}

	void Texture::setWrapping(GLenum s, GLenum t, GLenum r)
	{
		GLES_CHECK_ERROR(glBindTexture(mTarget, mID));
		GLES_CHECK_ERROR(glTexParameteri(mTarget, GL_TEXTURE_WRAP_S, s));
		GLES_CHECK_ERROR(glTexParameteri(mTarget, GL_TEXTURE_WRAP_T, t));
		GLES_CHECK_ERROR(glTexParameteri(mTarget, GL_TEXTURE_WRAP_R, r));
		GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
	}

	void Texture::setBorderColor(float r, float g, float b, float a)
	{
		std::array<float, 4> borderColor = { r, g, b, a };
		GLES_CHECK_ERROR(glBindTexture(mTarget, mID));
		GLES_CHECK_ERROR(glTexParameterfv(mTarget, GL_TEXTURE_BORDER_COLOR_NV, borderColor.data()));
		GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
	}

	void Texture::setMinFilter(GLenum filter)
	{
		GLES_CHECK_ERROR(glBindTexture(mTarget, mID));
		GLES_CHECK_ERROR(glTexParameteri(mTarget, GL_TEXTURE_MIN_FILTER, filter));
		GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
	}

	void Texture::setMagFilter(GLenum filter)
	{
		GLES_CHECK_ERROR(glBindTexture(mTarget, mID));
		GLES_CHECK_ERROR(glTexParameteri(mTarget, GL_TEXTURE_MAG_FILTER, filter));
		GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
	}

	void Texture::bindImage(uint32_t unit, uint32_t mipLevel, uint32_t layer, GLenum access, GLenum format)
	{
		bind(unit);

		if (mArraySize > 1)
			glBindImageTexture(unit, mID, mipLevel, GL_TRUE, layer, access, format);
		else
			glBindImageTexture(unit, mID, mipLevel, GL_FALSE, 0, access, format);
	}

	void Texture::setCompareMode(GLenum mode)
	{
		GLES_CHECK_ERROR(glBindTexture(mTarget, mID));
		GLES_CHECK_ERROR(glTexParameteri(mTarget, GL_TEXTURE_COMPARE_MODE, mode));
		GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
	}

	void Texture::setCompareFunc(GLenum func)
	{
		GLES_CHECK_ERROR(glBindTexture(mTarget, mID));
		GLES_CHECK_ERROR(glTexParameteri(mTarget, GL_TEXTURE_COMPARE_FUNC, func));
		GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
	}

	// ------------------------------------------------------------------------------------------------------------------------------------------

	/*
	Texture::Texture()
	{

	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &ID);
	}

	Texture* Texture::createWithFile(const std::string& path, Type type)
	{
		Texture* texture = new (std::nothrow) Texture();
		if (texture && texture->initWithFile(path, type))
		{
			return texture;
		}
		delete(texture);
		return nullptr;
	}

	GLuint Texture::getID() const
	{
		return ID;
	}

	glm::vec2 Texture::getSize() const
	{
		return glm::vec2(width, height);
	}

	GLint Texture::getChannelCount() const
	{
		return channelCount;
	}

	GLenum Texture::getFormat() const
	{
		return format;
	}

	std::string Texture::getFilePath() const
	{
		return filePath;
	}

	Texture::Type Texture::getType() const
	{
		return type;
	}

	GLboolean Texture::initWithFile(const std::string& path, Texture::Type type)
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

	TextureCube::TextureCube()
	{

	}

	TextureCube::~TextureCube()
	{
		glDeleteTextures(1, &ID);
	}

	TextureCube* TextureCube::createWithFiles(const std::vector<std::string>& filePaths)
	{
		TextureCube* textureCube = new (std::nothrow) TextureCube();
		if (textureCube && textureCube->initWithFiles(filePaths))
		{
			return textureCube;
		}
		delete(textureCube);
		return nullptr;
	}

	GLuint TextureCube::getID() const
	{
		return ID;
	}

	GLboolean TextureCube::initWithFiles(const std::vector<std::string>& filePaths)
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
	*/
}