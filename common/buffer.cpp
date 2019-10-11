#include "buffer.h"

namespace es
{
	Buffer::Buffer(GLenum type, GLenum usage, std::size_t size, void* data)
		:mType(type),
		 mSize(size)
	{
		GLES_CHECK_ERROR(glGenBuffers(1, &mID));
		GLES_CHECK_ERROR(glBindBuffer(type, mID));
		GLES_CHECK_ERROR(glBufferData(type, size, data, usage));
		GLES_CHECK_ERROR(glBindBuffer(type, 0));
	}

	Buffer::~Buffer()
	{
		GLES_CHECK_ERROR(glDeleteBuffers(1, &mID));
	}

	Buffer* Buffer::createWithData(GLenum type, GLenum usage, std::size_t size, void* data)
	{
		Buffer* buffer = new (std::nothrow) Buffer(type, usage, size, data);
		if (buffer)
		{
			return buffer;
		}
		delete(buffer);
		return nullptr;
	}

	void Buffer::bind()
	{
		GLES_CHECK_ERROR(glBindBuffer(mType, mID));
	}

	void Buffer::bindBase(int index)
	{
		GLES_CHECK_ERROR(glBindBufferBase(mType, index, mID));
	}

	void Buffer::bindRange(int index, std::size_t offset, std::size_t size)
	{
		GLES_CHECK_ERROR(glBindBufferRange(mType, index, mID, offset, size));
	}

	void Buffer::unbind()
	{
		GLES_CHECK_ERROR(glBindBuffer(mType, 0));
	}

	void* Buffer::mapRange(GLenum access, std::size_t offset, std::size_t size)
	{
		GLES_CHECK_ERROR(glBindBuffer(mType, mID));
		GLES_CHECK_ERROR(void* ptr = glMapBufferRange(mType, offset, size, access));
		GLES_CHECK_ERROR(glBindBuffer(mType, 0));
		return ptr;
	}

	void Buffer::unMap()
	{
		GLES_CHECK_ERROR(glBindBuffer(mType, mID));
		GLES_CHECK_ERROR(glUnmapBuffer(mType));
		GLES_CHECK_ERROR(glBindBuffer(mType, 0));
	}

	void Buffer::setData(std::size_t offset, std::size_t size, void* data)
	{
		GLES_CHECK_ERROR(glBindBuffer(mType, mID));
		GLES_CHECK_ERROR(glBufferSubData(mType, offset, size, data));
		GLES_CHECK_ERROR(glBindBuffer(mType, 0));
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	VertexBuffer::VertexBuffer(GLenum usage, std::size_t size, void* data) : Buffer(GL_ARRAY_BUFFER, usage, size, data)
	{

	}

	VertexBuffer::~VertexBuffer()
	{

	}

	VertexBuffer* VertexBuffer::createWithData(GLenum usage, std::size_t size, void* data)
	{
		VertexBuffer* buffer = new (std::nothrow) VertexBuffer(usage, size, data);
		if (buffer)
		{
			return buffer;
		}
		delete(buffer);
		return nullptr;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	ElementBuffer::ElementBuffer(GLenum usage, std::size_t size, void* data) : Buffer(GL_ELEMENT_ARRAY_BUFFER, usage, size, data)
	{

	}

	ElementBuffer::~ElementBuffer()
	{

	}

	ElementBuffer* ElementBuffer::createWithData(GLenum usage, std::size_t size, void* data)
	{
		ElementBuffer* buffer = new (std::nothrow) ElementBuffer(usage, size, data);
		if (buffer)
		{
			return buffer;
		}
		delete(buffer);
		return nullptr;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	UniformBuffer::UniformBuffer(GLenum usage, std::size_t size, void* data) : Buffer(GL_UNIFORM_BUFFER, usage, size, data)
	{

	}

	UniformBuffer::~UniformBuffer()
	{

	}

	UniformBuffer* UniformBuffer::createWithData(GLenum usage, std::size_t size, void* data)
	{
		UniformBuffer* buffer = new (std::nothrow) UniformBuffer(usage, size, data);
		if (buffer)
		{
			return buffer;
		}
		delete(buffer);
		return nullptr;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	ShaderStorageBuffer::ShaderStorageBuffer(GLenum usage, std::size_t size, void* data) : Buffer(GL_SHADER_STORAGE_BUFFER, usage, size, data)
	{

	}

	ShaderStorageBuffer::~ShaderStorageBuffer()
	{

	}

	ShaderStorageBuffer* ShaderStorageBuffer::createWithData(GLenum usage, std::size_t size, void* data)
	{
		ShaderStorageBuffer* buffer = new (std::nothrow) ShaderStorageBuffer(usage, size, data);
		if (buffer)
		{
			return buffer;
		}
		delete(buffer);
		return nullptr;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	VertexArray::VertexArray(VertexBuffer* vbo, ElementBuffer* ebo, std::size_t vertexSize, const std::vector<VertexAttrib>& attribs)
	{
		GLES_CHECK_ERROR(glGenVertexArrays(1, &mID));
		GLES_CHECK_ERROR(glBindVertexArray(mID));
		
		vbo->bind();
		if (ebo)
		{
			ebo->bind();
		}

		for (std::size_t i = 0; i < attribs.size(); i++)
		{
			GLES_CHECK_ERROR(glEnableVertexAttribArray(i));

			if (attribs[i].type == GL_INT)
			{
				GLES_CHECK_ERROR(glVertexAttribIPointer(i, attribs[i].numSubElements, attribs[i].type, vertexSize, (void*)((uint64_t)attribs[i].offset)));
			}
			else
			{
				GLES_CHECK_ERROR(glVertexAttribPointer(i, attribs[i].numSubElements, attribs[i].type, attribs[i].normalized, vertexSize, (void*)((uint64_t)attribs[i].offset)));
			}
		}

		GLES_CHECK_ERROR(glBindVertexArray(0));
		
		vbo->unbind();

		if (ebo)
		{
			ebo->unbind();
		}
	}

	VertexArray::~VertexArray()
	{
		GLES_CHECK_ERROR(glDeleteVertexArrays(1, &mID));
	}

	VertexArray* VertexArray::createWithData(VertexBuffer* vbo, ElementBuffer* ebo, std::size_t vertexSize, const std::vector<VertexAttrib>& attribs)
	{
		VertexArray* vao = new (std::nothrow) VertexArray(vbo, ebo, vertexSize, attribs);
		if (vao)
		{
			return vao;
		}
		delete(vao);
		return nullptr;
	}

	void VertexArray::bind()
	{
		GLES_CHECK_ERROR(glBindVertexArray(mID));
	}

	void VertexArray::unbind()
	{
		GLES_CHECK_ERROR(glBindVertexArray(0));
	}

	GLuint VertexArray::getID() const
	{
		return mID;
	}
}
