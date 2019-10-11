#pragma once

#include <ogles.h>
#include <vector>

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
	protected:
		GLenum mType;
		GLuint mID;
		size_t mSize;
	protected:
		Buffer(GLenum type, GLenum usage, std::size_t size, void* data);
		virtual ~Buffer();
	};

	class VertexBuffer : public Buffer
	{
	public:
		static VertexBuffer* createWithData(GLenum usage, std::size_t size, void* data = nullptr);
	private:
		VertexBuffer(GLenum usage, std::size_t size, void* data);
		~VertexBuffer();
	};

	class ElementBuffer : public Buffer
	{
	public:
		static ElementBuffer* createWithData(GLenum usage, std::size_t size, void* data = nullptr);
	private:
		ElementBuffer(GLenum usage, std::size_t size, void* data);
		~ElementBuffer();
	};

	class UniformBuffer : public Buffer
	{
	public:
		static UniformBuffer* createWithData(GLenum usage, std::size_t size, void* data = nullptr);
	private:
		UniformBuffer(GLenum usage, std::size_t size, void* data);
		~UniformBuffer();
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
		static VertexArray* createWithData(VertexBuffer* vbo, ElementBuffer* ebo, std::size_t vertexSize, std::vector<VertexAttrib> attribs);

		void bind();
		void unbind();

		GLuint getID() const;
	private:
		VertexArray(VertexBuffer* vbo, ElementBuffer* ebo, std::size_t vertexSize, std::vector<VertexAttrib> attribs);
		~VertexArray();

		GLuint mID;
	};
}