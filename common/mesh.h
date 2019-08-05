#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <optional>

using namespace std;

namespace es
{
	struct Vertex
	{
		std::optional<glm::vec3> Position;
		std::optional<glm::vec2> TexCoords;
		std::optional<glm::vec3> Normal;
		std::optional<glm::vec3> Tangent;
		std::optional<glm::vec3> Bitangent;
		std::optional<glm::vec4> Color;
	};

	class Mesh : public Object
	{
	public:
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;

		enum class DrawType
		{
			Arrays,
			Arrays_Indirect,
			Arrays_Instanced,
			Elements,
			Elements_Restart_Index
		};
		
		Mesh() = default;
		~Mesh()
		{
			if (material != nullptr)
			{
				delete(material);
				material = nullptr;
			}
			glDeleteBuffers(1, &EBO);
			glDeleteBuffers(1, &VAO);
			glDeleteVertexArrays(1, &VAO);
		}

		static Mesh* createWithData(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, DrawType type, Material* material = nullptr)
		{
			Mesh* mesh = new (std::nothrow) Mesh();
			if (mesh && mesh->initWithData(vertices, indices, type, material))
			{
				return mesh;
			}
			delete(mesh);
			return nullptr;
		}

		virtual void Draw() override
		{
			if (vertices.empty())
				return;

			if (material != nullptr)
			{
				material->apply();
			}

			glBindVertexArray(VAO);
			switch (drawType)
			{
				case DrawType::Arrays:
				{
					glDrawArrays(GL_TRIANGLES, 0, vertices.size());
					break;
				}
				case DrawType::Arrays_Indirect:
					break;
				case DrawType::Arrays_Instanced:
					break;
				case DrawType::Elements:
				{
					glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
					break;
				}
				case DrawType::Elements_Restart_Index:
				{
					// enable primitive restart
					glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
					glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
					// disable primitive restart
					glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
					break;
				}
			}
			glBindVertexArray(0);
			glActiveTexture(GL_TEXTURE0);
		}

		void setDrawType(DrawType type)
		{
			this->drawType = type;
		}

		void setMaterial(Material* material)
		{
			if (this->material != nullptr)
			{
				delete(this->material);
			}
			this->material = material;
		}

		Material* getMaterial()
		{
			return this->material;
		}

	private:
		GLuint VAO = 0;
		GLuint VBO = 0;;
		GLuint EBO = 0;
		DrawType drawType = DrawType::Elements;

		Material* material;

		bool initWithData(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, DrawType type, Material* material)
		{
			this->vertices = vertices;
			this->indices = indices;
			this->drawType = type;
			this->material = material;

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

			if (!indices.empty())
			{
				glGenBuffers(1, &EBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
			}

			GLushort attrLocation = 0;
			if (vertices[0].Position.has_value())
			{
				glVertexAttribPointer(attrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
				glEnableVertexAttribArray(attrLocation++);
			}

			if (vertices[0].TexCoords.has_value())
			{
				glVertexAttribPointer(attrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
				glEnableVertexAttribArray(attrLocation++);
			}

			if (vertices[0].Normal.has_value())
			{
				glVertexAttribPointer(attrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
				glEnableVertexAttribArray(attrLocation++);
			}

			if (vertices[0].Tangent.has_value())
			{
				glVertexAttribPointer(attrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
				glEnableVertexAttribArray(attrLocation++);
			}

			if (vertices[0].Bitangent.has_value())
			{
				glVertexAttribPointer(attrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
				glEnableVertexAttribArray(attrLocation++);
			}

			if (vertices[0].Color.has_value())
			{
				glVertexAttribPointer(attrLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
				glEnableVertexAttribArray(attrLocation);
			}
			
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			return true;
		}
	};
}