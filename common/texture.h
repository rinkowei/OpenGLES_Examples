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
		uint32_t getArraySize();

		void setWrapping(GLenum s, GLenum t, GLenum r);
		void setBorderColor(float r, float g, float b, float a);
		void setMinFilter(GLenum filter);
		void setMagFilter(GLenum filter);
		void setCompareMode(GLenum mode);
		void setCompareFunc(GLenum func);

		virtual void resize(uint32_t mipLevel, uint32_t w, uint32_t h) = 0;
	protected:
		GLuint mID;
		GLenum mTarget;
		GLenum mInternalFormat;
		GLenum mFormat;
		GLenum mType;
		GLuint mComponents;
		uint32_t mArraySize;
	};

	class Texture2D : public Texture
	{
	public:
		Texture2D(std::string path, int mipLevels = 1, bool srgb = true);
		Texture2D(uint32_t w, uint32_t h, uint32_t arraySize, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type);
		~Texture2D();

		static std::shared_ptr<Texture2D> createFromFile(std::string path, int mipLevels = 1, bool srgb = true);

		static std::shared_ptr<Texture2D> createFromData(uint32_t w, uint32_t h, uint32_t arraySize, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type);

		void setData(int arrayIndex, int mipLevel, void* data);

		virtual void resize(uint32_t mipLevel, uint32_t w, uint32_t h);

		uint32_t getWidth();
		uint32_t getHeight();
		uint32_t getMipLevels();
		uint32_t getNumSamples();
	private:
		void initFromFile(std::string path, int mipLevels, bool srgb);
		void initFromData(uint32_t w, uint32_t h, uint32_t arraySize, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type);

		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mMipLevels;
		uint32_t mNumSamples;

		static std::unordered_map<std::string, std::shared_ptr<Texture2D>> mTexture2DCache;
	};

	class TextureCube : public Texture
	{
	public:
		TextureCube(std::vector<std::string> paths, int mipLevels = 1, bool srgb = true);
		TextureCube(const std::string& name, uint32_t w, uint32_t h, int32_t mipLevels, GLenum internalFormat, GLenum format, GLenum type, void* data = nullptr);
		~TextureCube();

		static std::shared_ptr<TextureCube> createFromFiles(std::vector<std::string> paths, int mipLevels = 1, bool srgb = true);
		static std::shared_ptr<TextureCube> createFromData(const std::string& name, uint32_t w, uint32_t h, int32_t mipLevels, GLenum internalFormat, GLenum format, GLenum type, void* data = nullptr);

		void setData(int faceIndex, int layerIndex, int mipLevel, void* data);

		virtual void resize(uint32_t mipLevel, uint32_t w, uint32_t h);

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