#pragma once

#include "ogles.h"

namespace es
{
	class Buffer
	{
	public:
		Buffer(GLenum type, GLenum usage, size_t size, void* data);
		virtual ~Buffer();

		void bind();
		void bindBase(int index);
		void bindRange(int index, size_t offset, size_t size);
		void unbind();

		void* map(GLenum access);
		void* mapRange(GLenum access, size_t offset, size_t size);
		void unMap();

		void setData(size_t offset, size_t size, void* data);
	protected:
		GLenum mType;
		GLuint mGLBuffer;
		size_t mSize;
	};
}