#pragma once

#include <ogles.h>
#include <vector>
#include <memory>

namespace es
{
	class Buffer
	{
	public:
		static Buffer* createWithData(GLenum type, GLenum usage, std::size_t size, void* data);

		void bind();
		void bindBase(int index);
		void bindRange(int index, std::size_t offset, std::size_t size);
		void unbind();

		void* mapRange(GLenum access, std::size_t offset, std::size_t size);
		void unMap();

		void setData(std::size_t offset, std::size_t size, void* data);

		virtual ~Buffer();
	protected:
		GLenum mType;
		GLuint mID;
		size_t mSize;
	protected:
		Buffer(GLenum type, GLenum usage, std::size_t size, void* data);
	};

	class VertexBuffer : public Buffer
	{
	public:
		~VertexBuffer();

		template<typename... T>
		static std::unique_ptr<VertexBuffer> createWithData(T&&... args);
	protected:
		VertexBuffer(const VertexBuffer&) = delete;
		const VertexBuffer& operator=(const VertexBuffer&) = delete;
	private:
		VertexBuffer(GLenum usage, std::size_t size, void* data);
	};

	class ElementBuffer : public Buffer
	{
	public:
		~ElementBuffer();

		template<typename... T>
		static std::unique_ptr<ElementBuffer> createWithData(T &&... args);
	protected:
		ElementBuffer(const ElementBuffer&) = delete;
		const ElementBuffer& operator=(const ElementBuffer&) = delete;
	private:
		ElementBuffer(GLenum usage, std::size_t size, void* data);
	};

	class UniformBuffer : public Buffer
	{
	public:
		static UniformBuffer* createWithData(GLenum usage, std::size_t size, void* data = nullptr);
	private:
		UniformBuffer(GLenum usage, std::size_t size, void* data);
		~UniformBuffer();
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
		~VertexArray();

		template<typename... T>
		static std::unique_ptr<VertexArray> createWithData(T &&... args);

		void bind();
		void unbind();

		GLuint getID() const;
	protected:
		VertexArray(const VertexArray&) = delete;
		const VertexArray& operator=(const VertexArray&) = delete;
	private:
		VertexArray(VertexBuffer* vbo, ElementBuffer* ebo, std::size_t vertexSize, const std::vector<VertexAttrib>& attribs);

		GLuint mID;
	};
}