#pragma once

#include "ogles.h"
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <optional>

#include "material.h"
#include "world.h"

using namespace std;

namespace es
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoords;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
		glm::vec3 Color;
	};

	class Mesh : public Object
	{
	public:
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;

		enum class DrawType
		{
			ARRAYS,
			ARRAYS_INDIRECT,
			ARRAYS_INSTANCED,
			ELEMENTS,
			ELEMENTS_RESTART_INDEX
		};
		
		Mesh();

		~Mesh();

		static Mesh* createWithData(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, std::shared_ptr<Material> material, Mesh::DrawType type = Mesh::DrawType::ELEMENTS);

		virtual GLvoid render(float deltaTime) override;

		virtual GLvoid update(float deltaTime) override;

		GLvoid setDrawType(DrawType type);

		GLvoid setMaterial(std::shared_ptr<Material> material);

		std::shared_ptr<Material> getMaterial() const;

	private:
		GLuint VAO = 0;
		GLuint VBO = 0;;
		GLuint EBO = 0;
		DrawType drawType = DrawType::ELEMENTS;

		std::shared_ptr<Material> material;

		GLboolean initWithData(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, std::shared_ptr<Material> material, Mesh::DrawType type);
	};
}