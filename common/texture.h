#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <glm/glm.hpp>
#include <ogles.h>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <array>
#include <unordered_map>

namespace es
{
	class Image
	{
	public:
		template<typename T>
		struct Pixel
		{
			Pixel(uint32_t channel) : channel(channel)
			{

			};
			Pixel(T r, T g, T b) : r(r), g(g), b(b), channel(3)
			{

			};
			Pixel(T r, T g, T b, T a) : r(r), g(g), b(b), a(a), channel(4)
			{

			};

			T& operator[](uint32_t idx)
			{
				switch (idx)
				{
				case 0:
					return r;
				case 1:
					return g;
				case 2:
					return b;
				case 3:
					return a;
				}
			}

			const T& operator[](uint32_t idx) const
			{
				switch (idx)
				{
				case 0:
					return r;
				case 1:
					return g;
				case 2:
					return b;
				case 3:
					return a;
				}
			}

			T r;
			T g;
			T b;
			T a;
			const uint32_t channel;
		};

		Image();
		Image(uint32_t width, uint32_t height, uint32_t channel);
		Image(const std::string& filename, bool flipY = false);
		~Image();

		bool isValid() const;
		unsigned char* getData();
		const unsigned char* getDataConst() const;
		uint32_t getWidth() const;
		uint32_t getHeight() const;
		uint32_t getChannel() const;

		bool setPixel(uint32_t x, uint32_t y, const Pixel<unsigned char>& pixel);
		bool setPixel(uint32_t x, uint32_t y, const Pixel<float>& pixel);
		bool setPixel(uint32_t x, uint32_t y, const Pixel<double>& pixel);
		bool setPixel(uint32_t x, uint32_t y, const glm::vec3& pixel);

		Pixel<unsigned char> getPixel(uint32_t x, uint32_t y) const;
		Pixel<float> getPixelf(uint32_t x, uint32_t y) const;
		Pixel<double> getPixeld(uint32_t x, uint32_t y) const;

		unsigned char& at(uint32_t x, uint32_t y, uint32_t channel);
		const unsigned char& at(uint32_t x, uint32_t y, uint32_t channel) const;

		bool load(const std::string& filename, bool flipY = false);
		void genBuffer(uint32_t width, uint32_t height, uint32_t channel);
		void free();
		bool saveAsPNG(const std::string& filename, bool flipY = false) const;

		static Pixel<unsigned char> pixelf2uc(const Pixel<float>& pixel);
		static Pixel<unsigned char> pixeld2uc(const Pixel<double>& pixel);
		static Pixel<float> pixeluc2f(const Pixel<unsigned char>& pixel);
		static Pixel<double> pixeluc2d(const Pixel<unsigned char>& pixel);

		Image& operator=(const Image& img) = delete;
	private:
		enum class SrcType
		{
			Invalid,
			New,
			Stb
		};

		unsigned char* data;
		uint32_t width;
		uint32_t height;
		uint32_t channel;
		SrcType type;
	};

	class Texture
	{
	public:
		Texture();
		virtual ~Texture();

		// bind texture to specified texture uint
		void bind(uint32_t unit);
		void unbind(uint32_t unit);

		// bind to image unit
		void bindImage(uint32_t unit, uint32_t mipLevel, uint32_t layer, GLenum access, GLenum format);

		void generateMipmaps();

		GLuint getID();
		GLenum getTarget();
		GLenum getInternalFormat();
		GLenum getFormat();
		GLenum getType();

		void setWrapping(GLenum s, GLenum t, GLenum r);
		void setBorderColor(float r, float g, float b, float a);
		void setMinFilter(GLenum filter);
		void setMagFilter(GLenum filter);
		void setCompareMode(GLenum mode);
		void setCompareFunc(GLenum func);
	protected:
		GLuint mID;
		GLenum mTarget;
		GLenum mInternalFormat;
		GLenum mFormat;
		GLenum mType;
		GLuint mComponents;
	};

	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	class Texture2D : public Texture
	{
	public:
		Texture2D(std::string path, int mipLevels = 1, bool srgb = true, bool isFlipY = true);
		Texture2D(uint32_t w, uint32_t h, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type, bool isFixed = true);
		~Texture2D();

		static std::shared_ptr<Texture2D> createFromFile(std::string path, int mipLevels = 1, bool srgb = true, bool isFlipY = true);

		static std::shared_ptr<Texture2D> createFromData(uint32_t w, uint32_t h, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type, bool isFixed = true);

		void setData(uint32_t mipLevel, void* data);

		void resize(uint32_t mipLevel, uint32_t w, uint32_t h);

		uint32_t getWidth();
		uint32_t getHeight();
		uint32_t getMipLevels();
		uint32_t getNumSamples();
		bool getFixed() const;
	private:
		void initFromFile(std::string path, int mipLevels, bool srgb, bool isFlipY);
		void initFromData(uint32_t w, uint32_t h, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type, bool isFixed);

		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mMipLevels;
		uint32_t mNumSamples;
		bool mFixed;

		static std::unordered_map<std::string, std::shared_ptr<Texture2D>> mTexture2DCache;
	};

	class Texture2DArray : public Texture
	{
	public:
		Texture2DArray(uint32_t w, uint32_t h, uint32_t d, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type, bool isFixed = true);
		~Texture2DArray();

		static std::shared_ptr<Texture2DArray> createFromData(uint32_t w, uint32_t h, uint32_t d, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type, bool isFixed = true);

		uint32_t getWidth() const;
		uint32_t getHeight() const;
		uint32_t getDepth() const;
		uint32_t getMipLevels() const;
		uint32_t getNumSamples() const;
		bool getFixed() const;

		void resize(uint32_t mipLevel, uint32_t w, uint32_t h, uint32_t d);
	private:
		void initFromData(uint32_t w, uint32_t h, uint32_t d, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type, bool isFixed);

		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mDepth;
		uint32_t mMipLevels;
		uint32_t mNumSamples;
		bool mFixed;

		static std::unordered_map<std::string, std::shared_ptr<Texture2DArray>> mTexture2DArrayCache;
	};

	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	class TextureCube : public Texture
	{
	public:
		TextureCube(std::vector<std::string> paths, int mipLevels = 1, bool srgb = true);
		TextureCube(const std::string& name, uint32_t w, uint32_t h, int32_t mipLevels, GLenum internalFormat, GLenum format, GLenum type, void* data = nullptr);
		~TextureCube();

		static std::shared_ptr<TextureCube> createFromFiles(std::vector<std::string> paths, int mipLevels = 1, bool srgb = true);
		static std::shared_ptr<TextureCube> createFromData(const std::string& name, uint32_t w, uint32_t h, int32_t mipLevels, GLenum internalFormat, GLenum format, GLenum type, void* data = nullptr);

		void setData(int faceIndex, int layerIndex, int mipLevel, void* data);

		void resize(uint32_t mipLevel, uint32_t w, uint32_t h);

		uint32_t getWidth();
		uint32_t getHeight();
		uint32_t getMipLevels();
	private:
		bool initFromFiles(std::vector<std::string> paths, int mipLevels, bool srgb);
		bool initFromData(const std::string& name, uint32_t w, uint32_t h, int32_t mipLevels, GLenum internalFormat, GLenum format, GLenum type, void* data);

		std::string mName;

		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mMipLevels;

		static std::unordered_map<std::string, std::shared_ptr<TextureCube>> mTextureCubeCache;
	};
}

#endif