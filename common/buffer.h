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
		VertexBuffer(GLenum usage, std::size_t size, void* data);
		~VertexBuffer();

		template<typename... T>
		static std::unique_ptr<VertexBuffer> createWithData(T&&... args)
		{
			return std::make_unique<VertexBuffer>(std::forward<T>(args)...);
		}

		VertexBuffer(const VertexBuffer&) = delete;
		const VertexBuffer& operator=(const VertexBuffer&) = delete;
	};

	class ElementBuffer : public Buffer
	{
	public:
		ElementBuffer(GLenum usage, std::size_t size, void* data);
		~ElementBuffer();

		template<typename... T>
		static std::unique_ptr<ElementBuffer> createWithData(T&&... args)
		{
			return std::make_unique<ElementBuffer>(std::forward<T>(args)...);
		}

		ElementBuffer(const ElementBuffer&) = delete;
		const ElementBuffer& operator=(const ElementBuffer&) = delete;
	};

	class InstanceBuffer : public Buffer
	{
	public:
		InstanceBuffer(GLenum usage, std::size_t size, void* data);
		~InstanceBuffer();

		template<typename... T>
		static std::unique_ptr<InstanceBuffer> createWithData(T&&... args)
		{
			return std::make_unique<InstanceBuffer>(std::forward<T>(args)...);
		}

		InstanceBuffer(const InstanceBuffer&) = delete;
		const InstanceBuffer& operator=(const InstanceBuffer&) = delete;
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
		VertexArray(VertexBuffer* vbo, ElementBuffer* ebo, std::size_t vertexSize, const std::vector<VertexAttrib>& attribs);
		~VertexArray();

		template<typename... T>
		static std::unique_ptr<VertexArray> createWithData(T&&... args)
		{
			return std::make_unique<VertexArray>(std::forward<T>(args)...);
		}

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
}