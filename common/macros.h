#ifndef MACROS_H_
#define MACROS_H_

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#define IMGUI_IMPL_OPENGL_ES3
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM <GLES3/gl31.h>

#define SDL_MAIN_HANDLED

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#define ENABLE_GL_ERROR_CHECK
#ifdef ENABLE_GL_ERROR_CHECK
#define GLES_CHECK_ERROR(x)                                                         \
	x;																		        \
	{																			    \
		GLenum err(glGetError());                                                   \
		while (err != GL_NO_ERROR)                                                  \
		{                                                                           \
			std::string error;                                                      \
																					\
			switch (err)                                                            \
			{                                                                       \
			case GL_INVALID_OPERATION:                                              \
				error = "invalid operation";                                        \
				break;                                                              \
			case GL_INVALID_ENUM:                                                   \
				error = "invalid enum";                                             \
				break;                                                              \
			case GL_INVALID_VALUE:                                                  \
				error = "invalid value";                                            \
				break;                                                              \
			case GL_OUT_OF_MEMORY:                                                  \
				error = "out of memory";                                            \
				break;                                                              \
			case GL_INVALID_FRAMEBUFFER_OPERATION:                                  \
				error = "invalid framebuffer operation";                            \
				break;                                                              \
			}                                                                       \
																					\
			std::string formattedError = "OpenGL ES : ";                            \
			formattedError = formattedError + error;                                \
			formattedError = formattedError + ", line : ";                          \
			formattedError = formattedError + std::to_string(__LINE__);             \
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, formattedError.c_str());           \
			err = glGetError();                                                     \
		}																			\
	}																				\

#else
#define GLES_CHECK_ERROR(x)   x

#endif

#endif