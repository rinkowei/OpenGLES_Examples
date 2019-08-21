#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <iostream>

using namespace es;

namespace es
{
	class Framebuffer
	{
	public:
		enum class AttachmentType
		{
			ColorBuffer_1 = 0x1,
			ColorBuffer_2 = 0x2,
			ColorBuffer_3 = 0x4,
			ColorBuffer_4 = 0x8,
			RenderBuffer  = 0x10
		};

		Framebuffer() = default;
		~Framebuffer()
		{
			glDeleteFramebuffers(1, &ID);
		}

		static Framebuffer* create(uint32_t width, uint32_t height, uint32_t flags)
		{
			Framebuffer* framebuffer = new (std::nothrow) Framebuffer();
			if (framebuffer && framebuffer->init(width, height, flags))
			{
				return framebuffer;
			}
			delete(framebuffer);
			framebuffer = nullptr;
			return nullptr;
		}

		void apply()
		{
			if (completed)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, ID);
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			}
			else
			{
				std::cout << "failed to bind framebuffer!" << std::endl;
			}
		}
	private:
		GLuint ID = 0;
		GLuint width = 0;
		GLuint height = 0;
		GLboolean completed = false;

		GLboolean init(uint32_t width, uint32_t height, uint32_t flags)
		{
			this->width = width;
			this->height = height;

			glGenFramebuffers(1, &ID);
			glBindFramebuffer(GL_FRAMEBUFFER, ID);

			uint8_t colorBufferCount = 0;
			if (static_cast<uint32_t>(AttachmentType::ColorBuffer_1) & flags)
				colorBufferCount = 1;
			else if (static_cast<uint32_t>(AttachmentType::ColorBuffer_2) & flags)
				colorBufferCount = 2;
			else if (static_cast<uint32_t>(AttachmentType::ColorBuffer_3) & flags)
				colorBufferCount = 3;
			else if (static_cast<uint32_t>(AttachmentType::ColorBuffer_4) & flags)
				colorBufferCount = 4;

			for (uint8_t i = 0; i < colorBufferCount; i++)
			{
				GLuint colorAttachment;
				glGenTextures(1, &colorAttachment);
				glBindTexture(GL_TEXTURE_2D, colorAttachment);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorAttachment, 0);
			}

			if (static_cast<uint32_t>(AttachmentType::RenderBuffer) & flags)
			{
				GLuint renderbuffer;
				glGenRenderbuffers(1, &renderbuffer);
				glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
			}

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				std::cout << "failed to create framebuffer!" << std::endl;
				completed = false;
			}
			else
			{
				completed = true;
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			return completed;
		}
	};
}