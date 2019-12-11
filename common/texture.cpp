#include "texture.h"
#include <stb_image.h>
#include <stb_image_write.h>
#include <utility.h>

namespace es
{
	Image::Pixel<unsigned char> Image::pixelf2uc(const Pixel<float>& pixel)
	{
		Image::Pixel<unsigned char> rst(pixel.channel);
		for (uint32_t i = 0; i < pixel.channel; i++)
		{
			float mappedVal = 255.99f * pixel[i];
			rst[i] = static_cast<unsigned char>(mappedVal >= 256.0f ? 255 : static_cast<unsigned char>(mappedVal));
		}
		return rst;
	}

	Image::Pixel<unsigned char> Image::pixeld2uc(const Pixel<double>& pixel)
	{
		Image::Pixel<unsigned char> rst(pixel.channel);
		for (uint32_t i = 0; i < pixel.channel; i++)
		{
			float mappedVal = 255.99 * pixel[i];
			rst[i] = static_cast<unsigned char>(mappedVal >= 256.0 ? 255 : static_cast<unsigned char>(mappedVal));
		}
		return rst;
	}

	Image::Pixel<float> Image::pixeluc2f(const Pixel<unsigned char>& pixel)
	{
		Image::Pixel<float> rst(pixel.channel);
		for (uint32_t i = 0; i < pixel.channel; i++)
		{
			rst[i] = 1.0f / 255.0f * pixel[i];
		}
		return rst;
	}

	Image::Pixel<double> Image::pixeluc2d(const Pixel<unsigned char>& pixel)
	{
		Image::Pixel<double> rst(pixel.channel);
		for (uint32_t i = 0; i < pixel.channel; i++)
		{
			rst[i] = 1.0 / 255.0 * pixel[i];
		}
		return rst;
	}

	Image::Image()
		:data(nullptr),
		width(0),
		height(0),
		channel(0),
		type(SrcType::Invalid)
	{

	}

	Image::Image(uint32_t width, uint32_t height, uint32_t channel)
	{
		genBuffer(width, height, channel);
	}

	Image::Image(const std::string& filename, bool flipY)
	{
		data = nullptr;
		type = SrcType::Invalid;
		load(filename, flipY);
	}

	Image::~Image()
	{
		free();
	}

	bool Image::isValid() const
	{
		return data != nullptr && type != SrcType::Invalid;
	}

	unsigned char* Image::getData()
	{
		if (!isValid())
			return nullptr;
		return data;
	}

	const unsigned char* Image::getDataConst() const
	{
		if (!isValid())
			return nullptr;
		return data;
	}

	uint32_t Image::getWidth() const {
		return width;
	}

	uint32_t Image::getHeight() const {
		return height;
	}

	uint32_t Image::getChannel() const {
		return channel;
	}

	unsigned char& Image::at(uint32_t x, uint32_t y, uint32_t channel)
	{
		return data[(y * width + x) * this->channel + channel];
	}

	const unsigned char& Image::at(uint32_t x, uint32_t y, uint32_t channel) const
	{
		return data[(y * width + x) * this->channel + channel];
	}

	bool Image::setPixel(uint32_t x, uint32_t y, const Pixel<unsigned char>& pixel)
	{
		if (pixel.channel != this->channel)
			return false;

		for (uint32_t i = 0; i < channel; i++)
		{
			at(x, y, i) = pixel[i];
		}

		return true;
	}

	bool Image::setPixel(uint32_t x, uint32_t y, const Pixel<float>& pixel)
	{
		return setPixel(x, y, pixelf2uc(pixel));
	}

	bool Image::setPixel(uint32_t x, uint32_t y, const Pixel<double>& pixel)
	{
		return setPixel(x, y, pixeld2uc(pixel));
	}

	bool Image::setPixel(uint32_t x, uint32_t y, const glm::vec3& pixel)
	{
		return setPixel(x, y, pixelf2uc(Image::Pixel<float>(pixel.r, pixel.g, pixel.b)));
	}

	Image::Pixel<unsigned char> Image::getPixel(uint32_t x, uint32_t y) const
	{
		Pixel<unsigned char> rst(channel);
		for (uint32_t i = 0; i < channel; i++)
		{
			rst[i] = at(x, y, i);
		}
		return rst;
	}

	Image::Pixel<float> Image::getPixelf(uint32_t x, uint32_t y) const
	{
		return pixeluc2f(getPixel(x, y));
	}

	Image::Pixel<double> Image::getPixeld(uint32_t x, uint32_t y) const
	{
		return pixeluc2d(getPixel(x, y));
	}

	bool Image::load(const std::string& filename, bool flipY)
	{
		free();

		stbi_set_flip_vertically_on_load(flipY);

		int tempW, tempH, tempC;
		data = stbi_load(filename.c_str(), &tempW, &tempH, &tempC, 0);
		width = tempW;
		height = tempH;
		channel = tempC;

		if (!data)
		{
			type = SrcType::Invalid;
			return false;
		}

		type = SrcType::Stb;
		return true;
	}

	void Image::genBuffer(uint32_t width, uint32_t height, uint32_t channel)
	{
		free();

		this->width = width;
		this->height = height;
		this->channel = channel;

		data = new unsigned char[width * height * channel]();
		type = SrcType::New;
	}

	void Image::free()
	{
		if (!data)
		{
			switch (type)
			{
			case SrcType::New:
			{
				delete[] data;
				break;
			}
			case SrcType::Stb:
			{
				stbi_image_free(data);
				break;
			}
			case SrcType::Invalid:
			{
				break;
			}
			default:
				break;
			}

			width = height = channel = 0;
			data = nullptr;
			type = SrcType::Invalid;
		}
	}

	bool Image::saveAsPNG(const std::string& filename, bool flipY) const
	{
		stbi_flip_vertically_on_write(flipY);
		return stbi_write_png(filename.c_str(), width, height, channel, data, width * 3);
	}

	Texture::Texture()
		:mID(0),
		 mTarget(GL_TEXTURE_2D),
		 mInternalFormat(GL_RGBA8),
		 mFormat(GL_RGBA),
		 mType(GL_UNSIGNED_BYTE),
		 mComponents(4)
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
		GLES_CHECK_ERROR(glBindImageTexture(unit, mID, mipLevel, GL_FALSE, 0, access, format));
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

	Texture2D::Texture2D(std::string path, int mipLevels, bool srgb, bool isFlipY) : Texture()
	{
		initFromFile(path, mipLevels, srgb, isFlipY);
	}

	Texture2D::Texture2D(uint32_t w, uint32_t h, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type, bool isFixed) : Texture()
	{
		initFromData(w, h, mipLevels, numSamples, internalFormat, format, type, isFixed);
	}

	Texture2D::~Texture2D()
	{

	}

	std::shared_ptr<Texture2D> Texture2D::createFromFile(std::string path, int mipLevels, bool srgb, bool isFlipY)
	{
		if (mTexture2DCache.find(path) == mTexture2DCache.end())
		{
			std::shared_ptr<Texture2D> tex2d = std::make_shared<Texture2D>(path, mipLevels, srgb, isFlipY);
			mTexture2DCache[path] = tex2d;
			return tex2d;
		}
		else
		{
			return mTexture2DCache[path];
		}
	}

	std::shared_ptr<Texture2D> Texture2D::createFromData(uint32_t w, uint32_t h, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type, bool isFixed)
	{
		return std::make_shared<Texture2D>(w, h, mipLevels, numSamples, internalFormat, format, type, isFixed);
	}

	void Texture2D::setData(uint32_t mipLevel, void* data)
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

			if (mFixed)
			{
				GLES_CHECK_ERROR(glTexSubImage2D(mTarget, mipLevel, 0, 0, width, height, mFormat, mType, data));
			}
			else
			{
				GLES_CHECK_ERROR(glTexImage2D(mTarget, mipLevel, mInternalFormat, width, height, 0, mFormat, mType, data));
			}

			GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
		}
	}

	void Texture2D::initFromFile(std::string path, int mipLevels, bool srgb, bool isFlipY)
	{
		int width, height, components;
		void* data;
		bool ishdr = false;

		stbi_set_flip_vertically_on_load(isFlipY);
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
		GLenum type = GL_UNSIGNED_BYTE;

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
					internalFormat = GL_SRGB8;
				else
					internalFormat = GL_RGB8;

				if (ishdr)
				{
					internalFormat = GL_RGB32F;
					type = GL_FLOAT;
				}
				format = GL_RGB;
				break;
			}
			case 4:
			{
				if (srgb)
					internalFormat = GL_SRGB8_ALPHA8;
				else
					internalFormat = GL_RGBA8;

				if (ishdr)
				{
					internalFormat = GL_RGBA32F;
					type = GL_FLOAT;
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

		mInternalFormat = internalFormat;
		mFormat = format;
		mType = type;
		mWidth = width;
		mHeight = height;
		mNumSamples = 1;
		mFixed = true;
	    
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
		GLES_CHECK_ERROR(glTexStorage2D(mTarget, mMipLevels, mInternalFormat, mWidth, mHeight));
	
		for (int i = 0; i < mMipLevels; i++)
		{
			GLES_CHECK_ERROR(glTexSubImage2D(mTarget, i, 0, 0, width, height, mFormat, mType, data));
			width = max(1, (width / 2));
			height = max(1, (height / 2));
		}

		GLES_CHECK_ERROR(glTexParameteri(mTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GLES_CHECK_ERROR(glTexParameteri(mTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLES_CHECK_ERROR(glTexParameteri(mTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLES_CHECK_ERROR(glTexParameteri(mTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		GLES_CHECK_ERROR(glGenerateMipmap(mTarget));

		GLES_CHECK_ERROR(glBindTexture(mTarget, 0));

		stbi_image_free(data);
	}

	void Texture2D::initFromData(uint32_t w, uint32_t h, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type, bool isFixed)
	{
		mInternalFormat = internalFormat;
		mFormat = format;
		mType = type;
		mWidth = w;
		mHeight = h;
		mNumSamples = numSamples;
		mFixed = isFixed;

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

		if (mNumSamples > 1)
			mTarget = GL_TEXTURE_2D_MULTISAMPLE;
		else
			mTarget = GL_TEXTURE_2D;

		int width = mWidth;
		int height = mHeight;

		GLES_CHECK_ERROR(glBindTexture(mTarget, mID));

		if (mNumSamples > 1)
		{
			if (mFixed)
			{
				glTexStorage2DMultisample(mTarget, mNumSamples, mInternalFormat, mWidth, mHeight, true);
			}
		}
		else
		{
			if (mFixed)
			{
				glTexStorage2D(mTarget, mMipLevels, mInternalFormat, mWidth, mHeight);
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
		}

		GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
	}

	void Texture2D::resize(uint32_t mipLevel, uint32_t w, uint32_t h)
	{
		if (!mFixed)
		{
			GLES_CHECK_ERROR(glBindTexture(mTarget, mID));
			GLES_CHECK_ERROR(glTexImage2D(mTarget, mipLevel, mInternalFormat, w, h, 0, mFormat, mType, nullptr));
			GLES_CHECK_ERROR(glBindTexture(mTarget, 0));

			mWidth = w;
			mHeight = h;
		}
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

	bool Texture2D::getFixed() const
	{
		return mFixed;
	}

	// -------------------------------------------------------------------------------------------------------------------------------------------------

	std::unordered_map<std::string, std::shared_ptr<Texture2DArray>> Texture2DArray::mTexture2DArrayCache;

	Texture2DArray::Texture2DArray(uint32_t w, uint32_t h, uint32_t d, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type, bool isFixed) : Texture()
	{
		initFromData(w, h, d, mipLevels, numSamples, internalFormat, format, type, isFixed);
	}

	Texture2DArray::~Texture2DArray()
	{

	}

	std::shared_ptr<Texture2DArray> Texture2DArray::createFromData(uint32_t w, uint32_t h, uint32_t d, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type, bool isFixed)
	{
		return std::make_shared<Texture2DArray>(w, h, d, mipLevels, numSamples, internalFormat, format, type, isFixed);
	}

	void Texture2DArray::initFromData(uint32_t w, uint32_t h, uint32_t d, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type, bool isFixed)
	{
		mInternalFormat = internalFormat;
		mFormat = format;
		mType = type;
		mWidth = w;
		mHeight = h;
		mDepth = d;
		mNumSamples = numSamples;
		mFixed = isFixed;

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

		if (mNumSamples > 1)
			mTarget = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
		else
			mTarget = GL_TEXTURE_2D_ARRAY;

		int width = mWidth;
		int height = mHeight;

		GLES_CHECK_ERROR(glBindTexture(mTarget, mID));

		if (mNumSamples > 1)
		{
			if (mFixed)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "OpenGL ES : glTexStorage3DMultisample only can be supported by ES 3.2");
				//GLES_CHECK_ERROR(glTexStorage3DMultisample(mTarget, mNumSamples, mInternalFormat, mWidth, mHeight, mDepth, true));
			}
		}
		else
		{
			if (mFixed)
			{
				GLES_CHECK_ERROR(glTexStorage3D(mTarget, mMipLevels, mInternalFormat, mWidth, mHeight, mDepth));
			}
			else
			{
				for (int i = 0; i < mMipLevels; i++)
				{
					GLES_CHECK_ERROR(glTexImage3D(mTarget, i, mInternalFormat, width, height, mDepth, 0, mFormat, mType, nullptr));

					width = max(1, (width / 2));
					height = max(1, (height / 2));
				}
			}
		}
		GLES_CHECK_ERROR(glTexParameteri(mTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GLES_CHECK_ERROR(glTexParameteri(mTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLES_CHECK_ERROR(glTexParameteri(mTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLES_CHECK_ERROR(glTexParameteri(mTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		GLES_CHECK_ERROR(glGenerateMipmap(mTarget));

		GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
	}

	uint32_t Texture2DArray::getWidth() const
	{
		return mWidth;
	}

	uint32_t Texture2DArray::getHeight() const
	{
		return mHeight;
	}

	uint32_t Texture2DArray::getDepth() const
	{
		return mDepth;
	}

	uint32_t Texture2DArray::getMipLevels() const
	{
		return mMipLevels;
	}

	uint32_t Texture2DArray::getNumSamples() const
	{
		return mNumSamples;
	}

	bool Texture2DArray::getFixed() const
	{
		return mFixed;
	}

	void Texture2DArray::resize(uint32_t mipLevel, uint32_t w, uint32_t h, uint32_t d)
	{
		if (!mFixed)
		{
			GLES_CHECK_ERROR(glBindTexture(mTarget, mID));
			GLES_CHECK_ERROR(glTexImage3D(mTarget, mipLevel, mInternalFormat, w, h, d, 0, mFormat, mType, nullptr));
			GLES_CHECK_ERROR(glBindTexture(mTarget, 0));

			mWidth = w;
			mHeight = h;
			mDepth = d;
		}
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
			
			GLES_CHECK_ERROR(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mInternalFormat, width, height, 0, mFormat, mType, data));
			stbi_image_free(data);
		}
		GLES_CHECK_ERROR(glBindTexture(mTarget, 0));

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
			GLES_CHECK_ERROR(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mInternalFormat, mWidth, mHeight, 0, mFormat, mType, data));
		}
		GLES_CHECK_ERROR(glBindTexture(mTarget, 0));
		
		return true;
	}

	void TextureCube::resize(uint32_t mipLevel, uint32_t w, uint32_t h)
	{
		GLES_CHECK_ERROR(glBindTexture(mTarget, mID));
		for (GLuint i = 0; i < 6; i++)
		{
			GLES_CHECK_ERROR(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipLevel, mInternalFormat, w, h, 0, mFormat, mType, nullptr));
		}
		GLES_CHECK_ERROR(glBindTexture(mTarget, 0));

		mWidth = w;
		mHeight = h;
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