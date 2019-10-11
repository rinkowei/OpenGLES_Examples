#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "ogles.h"

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <array>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

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
		static Texture2D* createFromFile(std::string path, int mipLevels = -1, bool srgb = true);

		void setData(int arrayIndex, int mipLevel, void* data);

		uint32_t getWidth();
		uint32_t getHeight();
		uint32_t getMipLevels();
		uint32_t getNumSamples();
	private:
		Texture2D(std::string path, int mipLevels = -1, bool srgb = true);
		~Texture2D();

		void initFromFile(std::string path, int mipLevels, bool srgb);

		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mMipLevels;
		uint32_t mNumSamples;
	};

	class TextureCube : public Texture
	{
	public:
		TextureCube();
		~TextureCube();

		static TextureCube* createFromFiles(std::vector<std::string> paths, int mipLevels = -1, bool srgb = true);

		void setData(int faceIndex, int layerIndex, int mipLevel, void* data);

		uint32_t getWidth();
		uint32_t getHeight();
		uint32_t getMipLevels();
	private:
		bool initFromFiles(std::vector<std::string> paths, int mipLevels, bool srgb);

		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mMipLevels;
	};
	/*
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
	*/
}

#endif