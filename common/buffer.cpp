
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
}
