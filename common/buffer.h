#pragma once

#include "ogles.h"

namespace es
{
	class Buffer
	{
	public:
		Buffer(GLenum type, GLenum usage, std::size_t size, void* data);
		virtual ~Buffer();

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
	};
}