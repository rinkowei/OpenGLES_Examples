#ifndef TEXTRENDERER_H_
#define TEXTRENDERER_H_

#include <glm/glm.hpp>
#include <map>

#include "material.h"
#include "texture.h"
namespace es
{
	struct Character
	{
		std::shared_ptr<Texture2D> texture;
		glm::ivec2 size;
		glm::ivec2 bearing;
		uint32_t advance;
	};
	class TextRenderer
	{
	public:
		TextRenderer(uint32_t width, uint32_t height, std::shared_ptr<Material> mat);
		~TextRenderer();

		static std::unique_ptr<TextRenderer> create(uint32_t width, uint32_t height, std::shared_ptr<Material> mat);

		void load(std::string font, uint32_t fontSize);

		void text(std::string text, float x, float y, float scale, const glm::vec3& color = glm::vec3(1.0f));
	private:
		std::map<GLchar, Character> characters;
		std::shared_ptr<Material> material;

		GLuint VAO;
		GLuint VBO;
	};
}

#endif