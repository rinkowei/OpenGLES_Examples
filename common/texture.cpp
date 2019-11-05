#include "texture.h"
#include <stb_image.h>
#include <utility.h>

namespace es
{
	Texture::Texture()
		:mID(0),
		 mTarget(GL_TEXTURE_2D),
		 mInternalFormat(GL_RGBA8),
		 mFormat(GL_RGBA),
		 mType(GL_UNSIGNED_BYTE),
		 mComponents(4),
		 mArraySize(1)
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
		GLES_CHECK_ERROR(glTexParameterfv(mTarget, GL_TEXTURE_BORDER_COLOR_EXT, borderColor.data()));
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

	std::unordered_map<std::string, std::shared_ptr<Texture2D>> Texture2D::mTexture2DCache;

	Texture2D::Texture2D(std::string path, int mipLevels, bool srgb) : Texture()
	{
		initFromFile(path, mipLevels, srgb);
	}

	Texture2D::Texture2D(uint32_t w, uint32_t h, uint32_t arraySize, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type) : Texture()
	{
		initFromData(w, h, arraySize, mipLevels, numSamples, internalFormat, format, type);
	}

	Texture2D::~Texture2D()
	{

	}

	std::shared_ptr<Texture2D> Texture2D::createFromFile(std::string path, int mipLevels, bool srgb)
	{
		if (mTexture2DCache.find(path) == mTexture2DCache.end())
		{
			std::shared_ptr<Texture2D> tex2d = std::make_shared<Texture2D>(path, mipLevels, srgb);
			mTexture2DCache[path] = tex2d;
			return tex2d;
		}
		else
		{
			return mTexture2DCache[path];
		}
	}

	std::shared_ptr<Texture2D> Texture2D::createFromData(uint32_t w, uint32_t h, uint32_t arraySize, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type)
	{
		return std::make_shared<Texture2D>(w, h, arraySize, mipLevels, numSamples, internalFormat, format, type);
	}

	void Texture2D::setData(int arrayIndex, int mipLevel, void* data)
	{
		if (mNumSamples > 1)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "OpenGL ES : Multisampled texture data only can be assigned through shaders or FBOs");
		}
		else
		{
			int width = mWidth;
			int height = mHeight;

			for (int i = 0; i < mipLevel; i++)
			{
				width = max(1, width / 2);
				height = max(1, height / 2);
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

	void Texture2D::initFromFile(std::string path, int mipLevels, bool srgb)
	{
		int width, height, components;
		void* data;
		bool ishdr = false;
		if (Utility::fileExtension(path) == "hdr")
		{
			ishdr = true;
			data = (void*)stbi_loadf(path.c_str(), &width, &height, &components, 0);
		}
		else
		{
			data = (void*)stbi_load(path.c_str(), &width, &height, &components, 0);
		}

		if (!data)
		{
			stbi_image_free(data);
			return;
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

		if (ishdr)
		{
			mInternalFormat = GL_RGB16F;
			mFormat = GL_RGB;
			mType = GL_FLOAT;
		}
	    
		if (mipLevels == -1)
		{
			mMipLevels = 1;

			while (width > 1 && height > 1)
			{
				width = max(1, (width / 2));
				height = max(1, (height / 2));
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

				width = max(1, (width / 2));
				height = max(1, (height / 2));
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

				width = max(1, (width / 2));
				height = max(1, (height / 2));
			}

			GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
		}
	
		setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		setMagFilter(GL_LINEAR);

		setData(0, 0, data);
		generateMipmaps();

		stbi_image_free(data);
	}

	void Texture2D::initFromData(uint32_t w, uint32_t h, uint32_t arraySize, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type)
	{
		mArraySize = arraySize;
		mInternalFormat = internalFormat;
		mFormat = format;
		mType = type;
		mWidth = w;
		mHeight = h;
		mNumSamples = numSamples;

		if (mipLevels == -1)
		{
			mMipLevels = 1;

			int width = mWidth;
			int height = mHeight;

			while (width > 1 && height > 1)
			{
				width = max(1, (width / 2));
				height = max(1, (height / 2));
				mMipLevels++;
			}
		}
		else
		{
			mMipLevels = mipLevels;
		}

		if (arraySize > 1)
		{
			mTarget = GL_TEXTURE_2D_ARRAY;

			int width = mWidth;
			int height = mHeight;

			GLES_CHECK_ERROR(glBindTexture(mTarget, mID));

			for (int i = 0; i < mMipLevels; i++)
			{
				GLES_CHECK_ERROR(glTexImage3D(mTarget, i, mInternalFormat, width, height, mArraySize, 0, mFormat, mType, nullptr));

				width = max(1, (width / 2));
				height = max(1, (height / 2));
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

			int width = mWidth;
			int height = mHeight;

			GLES_CHECK_ERROR(glBindTexture(mTarget, mID));

			if (mNumSamples > 1)
			{

			}
			else
			{
				for (int i = 0; i < mMipLevels; i++)
				{
					GLES_CHECK_ERROR(glTexImage2D(mTarget, i, mInternalFormat, width, height, 0, mFormat, mType, nullptr));
					
					width = max(1, (width / 2));
					height = max(1, (height / 2));
				}
			}

			GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
		}

		setWrapping(GL_REPEAT, GL_REPEAT, GL_REPEAT);
		setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		setMagFilter(GL_LINEAR);

		generateMipmaps();
	}

	void Texture2D::resize(uint32_t mipLevel, uint32_t w, uint32_t h)
	{
		GLES_CHECK_ERROR(glBindTexture(mTarget, mID));
		GLES_CHECK_ERROR(glTexImage2D(mTarget, mipLevel, mInternalFormat, w, h, 0, mFormat, mType, nullptr));
		GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
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

	std::unordered_map<std::string, std::shared_ptr<TextureCube>> TextureCube::mTextureCubeCache;

	TextureCube::TextureCube(std::vector<std::string> paths, int mipLevels, bool srgb) : Texture()
	{
		initFromFiles(paths, mipLevels, srgb);
	}

	TextureCube::TextureCube(const std::string& name, uint32_t w, uint32_t h, int32_t mipLevels, GLenum internalFormat, GLenum format, GLenum type, void* data) : Texture()
	{
		initFromData(name, w, h, mipLevels, internalFormat, format, type, data);
	}

	TextureCube::~TextureCube()
	{

	}

	std::shared_ptr<TextureCube> TextureCube::createFromFiles(std::vector<std::string> paths, int mipLevels, bool srgb)
	{
		std::string directory = Utility::pathWithoutFile(paths.at(0));

		if (mTextureCubeCache.find(directory) == mTextureCubeCache.end())
		{
			std::shared_ptr<TextureCube> texCube = std::make_shared<TextureCube>(paths, mipLevels, srgb);
			mTextureCubeCache[directory] = texCube;
			return texCube;
		}
		else
		{
			return mTextureCubeCache[directory];
		}
	}

	std::shared_ptr<TextureCube> TextureCube::createFromData(const std::string& name, uint32_t w, uint32_t h, int32_t mipLevels, GLenum internalFormat, GLenum format, GLenum type, void* data)
	{
		if (mTextureCubeCache.find(name) == mTextureCubeCache.end())
		{
			std::shared_ptr<TextureCube> texCube = std::make_shared<TextureCube>(name, w, h, mipLevels, internalFormat, format, type, data);
			mTextureCubeCache[name] = texCube;
			return texCube;
		}
		else
		{
			return mTextureCubeCache[name];
		}
	}

	void TextureCube::setData(int faceIndex, int layerIndex, int mipLevel, void* data)
	{
		int width = mWidth;
		int height = mHeight;

		for (int i = 0; i < mipLevel; i++)
		{
			width = max(1, (width / 2));
			height = max(1, (height / 2));
		}

		GLES_CHECK_ERROR(glBindTexture(mTarget, mID));
		GLES_CHECK_ERROR(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex, mipLevel, mInternalFormat, width, height, 0, mFormat, mType, data));
		GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
	}

	bool TextureCube::initFromFiles(std::vector<std::string> paths, int mipLevels, bool srgb)
	{
		int width;
		int height;
		int components;
		void* data;

		bool ishdr = false;
		if (Utility::fileExtension(paths.at(0)) == "hdr")
		{
			ishdr = true;
			mType = GL_FLOAT;
		}
		else
		{
			ishdr = false;
			mType = GL_UNSIGNED_BYTE;
		}

		mTarget = GL_TEXTURE_CUBE_MAP;
		GLES_CHECK_ERROR(glBindTexture(mTarget, mID));
		for (std::size_t i = 0; i < paths.size(); i++)
		{
			if (ishdr)
			{
				data = (void*)stbi_loadf(paths.at(i).c_str(), &width, &height, &components, 0);
				if (components == 1)
				{
					mInternalFormat = GL_R32F;
					mFormat = GL_RED;
				}
				else if (components == 2)
				{
					mInternalFormat = GL_RG32F;
					mFormat = GL_RG;
				}
				else if (components == 3)
				{
					mInternalFormat = GL_RGB32F;
					mFormat = GL_RGB;
				}
				else if (components == 4)
				{
					mInternalFormat = GL_RGBA32F;
					mFormat = GL_RGBA;
				}
			}
			else
			{
				data = (void*)stbi_load(paths.at(i).c_str(), &width, &height, &components, 0);
				if (components == 1)
				{
					mInternalFormat = GL_R8;
					mFormat = GL_RED;
				}
				else if (components == 2)
				{
					mInternalFormat = GL_RG8;
					mFormat = GL_RG;
				}
				else if (components == 3)
				{
					if (srgb)
						mInternalFormat = GL_SRGB8;
					else
						mInternalFormat = GL_RGB8;
					mFormat = GL_RGB;
				}
				else if (components == 4)
				{
					if (srgb)
						mInternalFormat = GL_SRGB8_ALPHA8;
					else
						mInternalFormat = GL_RGBA8;
					mFormat = GL_RGBA;
				}
			}

			if (!data)
			{
				stbi_image_free(data);
				return nullptr;
			}
			
			mWidth = width;
			mHeight = height;
			
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mInternalFormat, width, height, 0, mFormat, mType, data);
			stbi_image_free(data);
		}
		GLES_CHECK_ERROR(glBindTexture(mTarget, 0));

		setMinFilter(GL_LINEAR);
		setMagFilter(GL_LINEAR);
		setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		return true;
	}

	bool TextureCube::initFromData(const std::string& name, uint32_t w, uint32_t h, int32_t mipLevels, GLenum internalFormat, GLenum format, GLenum type, void* data)
	{
		mName = name;
		mTarget = GL_TEXTURE_CUBE_MAP;
		mInternalFormat = internalFormat;
		mFormat = format;
		mType = type;
		mWidth = w;
		mHeight = h;
		mMipLevels = mipLevels;

		GLES_CHECK_ERROR(glBindTexture(mTarget, mID));
		for (GLuint i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mInternalFormat, w, h, 0, mFormat, mType, data);
		}
		GLES_CHECK_ERROR(glBindTexture(mTarget, 0));

		setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		setMagFilter(GL_LINEAR);
		setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		generateMipmaps();

		return true;
	}

	void TextureCube::resize(uint32_t mipLevel, uint32_t w, uint32_t h)
	{
		GLES_CHECK_ERROR(glBindTexture(mTarget, mID));
		for (GLuint i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipLevel, mInternalFormat, w, h, 0, mFormat, mType, nullptr);
		}
		GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
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
}