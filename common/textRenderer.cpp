#include "textRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace es
{
	TextRenderer::TextRenderer(uint32_t width, uint32_t height, std::shared_ptr<Material> mat)
	{
		material = mat;
		material->setUniform("projection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f));
		material->setUniform("glyphTexture", 0);

		GLES_CHECK_ERROR(glGenVertexArrays(1, &VAO));
		GLES_CHECK_ERROR(glGenBuffers(1, &VBO));
		GLES_CHECK_ERROR(glBindVertexArray(VAO));
		GLES_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, VBO));
		GLES_CHECK_ERROR(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW));

		GLES_CHECK_ERROR(glEnableVertexAttribArray(0));
		GLES_CHECK_ERROR(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0));

		GLES_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GLES_CHECK_ERROR(glBindVertexArray(0));
	}

	TextRenderer::~TextRenderer()
	{
		if (material != nullptr)
		{
			material.reset();
		}

		for (auto iter = characters.begin(); iter != characters.end(); iter++)
		{
			if (iter->second.texture != nullptr)
			{
				iter->second.texture.reset();
			}
		}
		characters.swap(std::map<GLchar, Character>());
	}

	std::unique_ptr<TextRenderer> TextRenderer::create(uint32_t width, uint32_t height, std::shared_ptr<Material> mat)
	{
		return std::make_unique<TextRenderer>(width, height, mat);
	}

	void TextRenderer::load(std::string font, uint32_t fontSize)
	{
		characters.clear();

		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "failed to init freetype library");
		}

		FT_Face face;
		if (FT_New_Face(ft, font.c_str(), 0, &face))
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "failed to load font : %s", font.c_str());
		}

		FT_Set_Pixel_Sizes(face, 0, fontSize);

		GLES_CHECK_ERROR(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

		for (GLubyte c = 0; c < 128; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "failed to load glyph : %c", c);
				continue;
			}

			std::shared_ptr<Texture2D> tex = Texture2D::createFromData(face->glyph->bitmap.width, face->glyph->bitmap.rows, 1, 1, GL_RED, GL_RED, GL_UNSIGNED_BYTE, false);
			tex->setData(0, face->glyph->bitmap.buffer);

			tex->setMinFilter(GL_LINEAR);
			tex->setMagFilter(GL_LINEAR);
			tex->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

			Character character = {
				tex,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			characters.insert(std::make_pair(c, character));
		}

		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}

	void TextRenderer::text(std::string text, float x, float y, float scale, const glm::vec3& color)
	{
		material->setUniform("textColor", color);
		material->apply();
		GLES_CHECK_ERROR(glActiveTexture(GL_TEXTURE0));
		GLES_CHECK_ERROR(glBindVertexArray(VAO));

		// enable blend
		GLES_CHECK_ERROR(glEnable(GL_BLEND));
		GLES_CHECK_ERROR(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		std::string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = characters[*c];

			float xpos = x + ch.bearing.x * scale;
			float ypos = y + (characters['H'].bearing.y - ch.bearing.y) * scale;

			float w = ch.size.x * scale;
			float h = ch.size.y * scale;

			std::array<float, 6 * 4> vertices = {
				// positions          // texture coordinates
				 xpos,     ypos + h,   0.0, 1.0,
				 xpos + w, ypos,       1.0, 0.0,
				 xpos,     ypos,       0.0, 0.0,

				 xpos,     ypos + h,   0.0, 1.0,
				 xpos + w, ypos + h,   1.0, 1.0,
				 xpos + w, ypos,       1.0, 0.0
			};

			GLES_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, ch.texture->getID()));
			GLES_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, VBO));
			GLES_CHECK_ERROR(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices.data()));

			GLES_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, 0));

			GLES_CHECK_ERROR(glDrawArrays(GL_TRIANGLES, 0, 6));
			// Now advance cursors for next glyph
			x += (ch.advance >> 6)* scale; // Bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
		}

		GLES_CHECK_ERROR(glDisable(GL_BLEND));

		material->unapply();

		GLES_CHECK_ERROR(glBindVertexArray(0));
		GLES_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, 0));
	}
}
