#include "mesh.h"

namespace es
{
	Mesh::Mesh()
	{

	}

	Mesh::~Mesh()
	{
		if (material != nullptr)
		{
			material.reset();
			material = nullptr;
		}
		glDeleteBuffers(1, &EBO);
		glDeleteBuffers(1, &VAO);
		glDeleteVertexArrays(1, &VAO);
	}

	Mesh* Mesh::createWithData(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, std::shared_ptr<Material> material, Mesh::DrawType type)
	{
		Mesh* mesh = new (std::nothrow) Mesh();
		if (mesh && mesh->initWithData(vertices, indices, material, type))
		{
			return mesh;
		}
		delete(mesh);
		return nullptr;
	}

	GLvoid Mesh::render(float deltaTime)
	{
		if (autoUpdated)
		{
			Object::update(deltaTime);
		}

		if (vertices.empty())
			return;

		if (material != nullptr)
		{
			material->apply();
			material->setMatrix4x4("model", model);
			material->setMatrix4x4("view", World::getWorld()->getDefaultCamera()->getViewMatrix());
			material->setMatrix4x4("projection", World::getWorld()->getDefaultCamera()->getProjectionMatrix());
		}

		glBindVertexArray(VAO);
		switch (drawType)
		{
			case DrawType::ARRAYS:
			{
				glDrawArrays(GL_TRIANGLES, 0, vertices.size());
				break;
			}
			case DrawType::ARRAYS_INDIRECT:
				break;
			case DrawType::ARRAYS_INSTANCED:
				break;
			case DrawType::ELEMENTS:
			{
				glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
				break;
			}
			case DrawType::ELEMENTS_RESTART_INDEX:
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
	}

	GLvoid Mesh::update(float deltaTime)
	{
		Object::update(deltaTime);
	}

	GLvoid Mesh::setDrawType(DrawType type)
	{
		this->drawType = type;
	}

	GLvoid Mesh::setMaterial(std::shared_ptr<Material> material)
	{
		if (!this->material)
		{
			this->material.reset();
		}
		this->material = material;
	}

	std::shared_ptr<Material> Mesh::getMaterial() const
	{
		return this->material;
	}

	GLboolean Mesh::initWithData(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, std::shared_ptr<Material> material, Mesh::DrawType type)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->material = material;
		this->drawType = type;

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

		glVertexAttribPointer(attrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
		glEnableVertexAttribArray(attrLocation++);

		glVertexAttribPointer(attrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		glEnableVertexAttribArray(attrLocation++);

		glVertexAttribPointer(attrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(attrLocation++);

		glVertexAttribPointer(attrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		glEnableVertexAttribArray(attrLocation++);

		glVertexAttribPointer(attrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		glEnableVertexAttribArray(attrLocation++);

		glVertexAttribPointer(attrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
		glEnableVertexAttribArray(attrLocation);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		return true;
	}
}