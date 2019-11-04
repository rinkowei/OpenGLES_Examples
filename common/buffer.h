#pragma once

#include <ogles.h>
#include <texture.h>
#include <program.h>
#include <vector>
#include <optional>
#include <memory>

namespace es
{
	class Buffer
	{
	public:
		Buffer(GLenum type);
		Buffer(GLenum type, GLenum usage, std::size_t size, void* data);
		virtual ~Buffer();

		static std::shared_ptr<Buffer> createWithData(GLenum type, GLenum usage, std::size_t size, void* data);

		void bind();
		void bindBase(GLuint index);
		void bindRange(int index, std::size_t offset, std::size_t size);
		void unbind();

		void* mapRange(GLenum access, std::size_t offset, std::size_t size);
		void unMap();

		void setData(GLintptr offset, GLsizeiptr size, void* data);

		GLuint getID() const;
	protected:
		GLenum mType;
		GLuint mID;
		size_t mSize;
	};

	class VertexBuffer : public Buffer
	{
	public:
		VertexBuffer(GLenum usage, std::size_t size, void* data);
		~VertexBuffer();

		static std::shared_ptr<VertexBuffer> createWithData(GLenum usage, std::size_t size, void* data);
	};

	class ElementBuffer : public Buffer
	{
	public:
		ElementBuffer(GLenum usage, std::size_t size, void* data);
		~ElementBuffer();

		static std::shared_ptr<ElementBuffer> createWithData(GLenum usage, std::size_t size, void* data);
	};

	class InstanceBuffer : public Buffer
	{
	public:
		InstanceBuffer(GLenum usage, std::size_t size, void* data);
		~InstanceBuffer();

		static std::shared_ptr<InstanceBuffer> createWithData(GLenum usage, std::size_t size, void* data);
	};

	class UniformBuffer : public Buffer
	{
	public:
		UniformBuffer(GLenum usage, std::size_t size, void* data = nullptr);
		UniformBuffer(GLenum usage, Program* program, const std::string& uniformBlockName, GLuint bindingPoint);
		~UniformBuffer();

		static std::unique_ptr<UniformBuffer> createWithData(GLenum usage, std::size_t size, void* data = nullptr);

		static std::unique_ptr<UniformBuffer> createWithData(GLenum usage, Program* program, const std::string& uniformBlockName, GLuint bindingPoint);
	};

	class ShaderStorageBuffer : public Buffer
	{
	public:
		static ShaderStorageBuffer* createWithData(GLenum usage, std::size_t size, void* data = nullptr);
	private:
		ShaderStorageBuffer(GLenum usage, std::size_t size, void* data);
		~ShaderStorageBuffer();
	};

	struct VertexAttrib
	{
		uint32_t numSubElements;
		uint32_t type;
		bool     normalized;
		uint32_t offset;
	};

	class VertexArray
	{
	public:
		VertexArray(VertexBuffer* vbo, ElementBuffer* ebo, std::size_t vertexSize, const std::vector<VertexAttrib>& attribs);
		~VertexArray();

		static std::shared_ptr<VertexArray> createWithData(VertexBuffer* vbo, ElementBuffer* ebo, std::size_t vertexSize, const std::vector<VertexAttrib>& attribs);

		void bind();
		void unbind();

		GLuint getID() const;

		GLuint getVertexAttribCount() const;

		VertexArray(const VertexArray&) = delete;
		const VertexArray& operator=(const VertexArray&) = delete;
	private:
		GLuint mID;
		GLuint mVertexAttribCount;
	};

	class Framebuffer
	{
	public:
		Framebuffer();
		~Framebuffer();

		static std::unique_ptr<Framebuffer> create();

		void bind();
		void unbind();

		void attachRenderTarget(uint32_t attachment, Texture* texture, uint32_t layer, uint32_t mipLevel, bool draw = true, bool read = true);

		void attachDepthRenderTarget(Texture* texture, uint32_t layer, uint32_t mipLevel);

		void attachDepthRenderTarget(TextureCube* texture, uint32_t face, uint32_t layer, uint32_t mipLevel);

		void attachDepthStencilTarget(uint32_t w, uint32_t h);

		GLuint getRenderBuffer() const;
	private:
		void checkStatus();

		std::vector<GLenum> mAttachments;

		GLuint mRenderBuffer;

		GLuint mID;
	};

	class Renderbuffer
	{
	public:
		Renderbuffer(GLenum internalFormat, uint32_t w, uint32_t h);
		~Renderbuffer();

		static std::unique_ptr<Renderbuffer> create(GLenum internalFormat, uint32_t w, uint32_t h);

		void bind();
		void unbind();
	private:
		GLenum mTarget;
		GLenum mInternalFormat;

		uint32_t mWidth;
		uint32_t mHeight;

		GLuint mID;
	};
}