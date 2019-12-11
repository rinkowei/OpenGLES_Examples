#ifndef TEXTURE_H_
#define TEXTURE_H_

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