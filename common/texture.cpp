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

	Texture2D::Texture2D() : Texture()
	{

	}

	Texture2D::~Texture2D()
	{

	}

	Texture2D* Texture2D::createFromFile(std::string path, int mipLevels, bool srgb)
	{
		Texture2D* texture = new (std::nothrow) Texture2D();
		if (texture && texture->initFromFile(path, mipLevels, srgb))
		{
			return texture;
		}
		delete(texture);
		return nullptr;
	}

	void Texture2D::setData(int arrayIndex, int mipLevel, void* data)
	{
		if (mNumSamples > 1)
		{
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "OpenGL ES : Multisampled texture data only can be assigned through shaders or FBOs");
		}
		else
		{
			int width = mWidth;
			int height = mHeight;

			for (int i = 0; i < mipLevel; i++)
			{
				width = std::max(1, width / 2);
				height = std::max(1, height / 2);
			}

			GLES_CHECK_ERROR(glBindTexture(mTarget, mID));

			if (mArraySize > 1)
			{
				GLES_CHECK_ERROR(glTexImage3D(mTarget, mipLevel, mInternalFormat, width, height, arrayIndex, 0, mFormat, mType, data));
			}
			else
			{
				GLES_CHECK_ERROR(glTexImage2D(mTarget, mipLevel, mInternalFormat, width, height, 0, mFormat, mType, data));
			}

			GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
		}
	}

	bool Texture2D::initFromFile(std::string path, int mipLevels, bool srgb)
	{
		int width, height, components;
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &components, 0);

		if (!data)
		{
			return false;
		}

		GLenum internalFormat, format;

		switch (components)
		{
			case 1:
			{
				internalFormat = GL_R8;
				format = GL_RED;
				break;
			}
			case 2:
			{
				internalFormat = GL_RG8;
				format = GL_RG;
				break;
			}
			case 3:
			{
				if (srgb)
				{
					internalFormat = GL_SRGB8;
				}
				else
				{
					internalFormat = GL_RGB8;
				}
				format = GL_RGB;
				break;
			}
			case 4:
			{
				if (srgb)
				{
					internalFormat = GL_SRGB8_ALPHA8;
				}
				else
				{
					internalFormat = GL_RGBA8;
				}
				format = GL_RGBA;
				break;
			}
			default:
			{
				internalFormat = GL_RGBA8;
				format = GL_RGBA;
				break;
			}
		}

		mArraySize = 1;
		mInternalFormat = internalFormat;
		mFormat = format;
		mType = GL_UNSIGNED_BYTE;
		mWidth = width;
		mHeight = height;
		mNumSamples = 1;

		if (mipLevels = -1)
		{
			mMipLevels = 1;

			while (width > 1 && height > 1)
			{
				width = std::max(1, (width / 2));
				height = std::max(1, (height / 2));
				mMipLevels++;
			}
		}
		else
		{
			mMipLevels = mipLevels;
		}

		if (mArraySize > 1)
		{
			mTarget = GL_TEXTURE_2D_ARRAY;

			width = mWidth;
			height = mHeight;

			GLES_CHECK_ERROR(glBindTexture(mTarget, mID));
			for (int i = 0; i < mMipLevels; i++)
			{
				GLES_CHECK_ERROR(glTexImage3D(mTarget, i, mInternalFormat, width, height, mArraySize, 0, mFormat, mType, nullptr));

				width = std::max(1, (width / 2));
				height = std::max(1, (height / 2));
			}

			GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
		}
		else
		{
			if (mNumSamples > 1)
			{
				mTarget = GL_TEXTURE_2D_MULTISAMPLE;
			}
			else
			{
				mTarget = GL_TEXTURE_2D;
			}

			width = mWidth;
			height = mHeight;

			GLES_CHECK_ERROR(glBindTexture(mTarget, mID));
			
			for (int i = 0; i < mMipLevels; i++)
			{
				GLES_CHECK_ERROR(glTexImage2D(mTarget, i, mInternalFormat, width, height, 0, mFormat, mType, nullptr));

				width = std::max(1, (width / 2));
				height = std::max(1, (height / 2));
			}

			GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
		}
		
		setWrapping(GL_REPEAT, GL_REPEAT, GL_REPEAT);
		setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		setMagFilter(GL_LINEAR);

		setData(0, 0, data);
		generateMipmaps();

		stbi_image_free(data);
	}

	uint32_t Texture2D::getWidth()
	{
		return mWidth;
	}

	uint32_t Texture2D::getHeight()
	{
		return mHeight;
	}

	uint32_t Texture2D::getMipLevels()
	{
		return mMipLevels;
	}

	uint32_t Texture2D::getNumSamples()
	{
		return mNumSamples;
	}

	// -------------------------------------------------------------------------------------------------------------------------------------------------

	TextureCube::TextureCube() : Texture()
	{

	}

	TextureCube::~TextureCube()
	{

	}

	TextureCube* TextureCube::createFromFiles(std::vector<std::string> paths, int mipLevels, bool srgb)
	{
		TextureCube* texture = new (std::nothrow) TextureCube();
		if (texture && texture->initFromFiles(paths, mipLevels, srgb))
		{
			return texture;
		}
		delete(texture);
		return nullptr;
	}

	void TextureCube::setData(int faceIndex, int layerIndex, int mipLevel, void* data)
	{

	}

	bool TextureCube::initFromFiles(std::vector<std::string> paths, int mipLevels, bool srgb)
	{
		return false;
	}

	uint32_t TextureCube::getWidth()
	{
		return mWidth;
	}

	uint32_t TextureCube::getHeight()
	{
		return mHeight;
	}

	uint32_t TextureCube::getMipLevels()
	{
		return mMipLevels;
	}

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