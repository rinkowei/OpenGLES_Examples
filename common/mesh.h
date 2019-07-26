#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"

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
		std::optional<glm::vec3> Color;
	};

	struct Texture
	{
		GLuint id;
		string type;
		string path;
	};

	class Mesh
	{
	public:
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Texture> textures;

		enum class DrawType
		{
			Arrays,
			Arrays_Indirect,
			Arrays_Instanced,
			Elements,
			Elements_Restart_Index
		};
		
		Mesh() = default;
		~Mesh() = default;

		static Mesh* createWithData(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures, DrawType type)
		{
			Mesh* mesh = new (std::nothrow) Mesh();
			if (mesh && mesh->initWithData(vertices, indices, textures, type))
			{
				return mesh;
			}
			delete(mesh);
			return nullptr;
		}

		void Draw(Shader* shader)
		{
			if (vertices.empty())
				return;

			if (!textures.empty())
			{
				GLuint diffuseMap = 1;
				GLuint specularMap = 1;
				GLuint NormalMap = 1;
				GLuint heightMap = 1;
				for (GLuint i = 0; i < textures.size(); i++)
				{
					glActiveTexture(GL_TEXTURE0 + i);
					string number;
					string name = textures[i].type;
					if (name == "diffuseMap")
					{
						number = std::to_string(diffuseMap++);
					}
					else if (name == "specularMap")
					{
						number = std::to_string(specularMap++);
					}
					else if (name == "normalMap")
					{
						number = std::to_string(NormalMap++);
					}
					else if (name == "heightMap")
					{
						number = std::to_string(heightMap++);
					}

					shader->setInt((name + number).c_str(), i);
					glBindTexture(GL_TEXTURE_2D, textures[i].id);
				}
			}

			shader->use();
			glBindVertexArray(VAO);
			switch (drawType)
			{
			case DrawType::Arrays:
				glDrawArrays(GL_TRIANGLES, 0, vertices.size());
				break;
			case DrawType::Arrays_Indirect:
				break;
			case DrawType::Arrays_Instanced:
				break;
			case DrawType::Elements:
				glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
				break;
			case DrawType::Elements_Restart_Index:
				break;
			}
			glBindVertexArray(0);
			glActiveTexture(GL_TEXTURE0);
		}

		void setDrawType(DrawType type)
		{
			this->drawType = type;
		}

	private:
		GLuint VAO = 0;
		GLuint VBO = 0;;
		GLuint EBO = 0;
		DrawType drawType = DrawType::Elements;

		bool initWithData(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures, DrawType type)
		{
			this->vertices = vertices;
			this->indices = indices;
			this->textures = textures;

			setDrawType(type);

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
				glVertexAttribPointer(attrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
				glEnableVertexAttribArray(attrLocation);
			}

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			return true;
		}
	};
}