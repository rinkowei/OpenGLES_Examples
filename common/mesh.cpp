#include "mesh.h"

namespace es
{
	Mesh::Mesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
	{
		mName = name;
		mVertices = vertices;
		mIndices = indices;

		mVBO = VertexBuffer::createWithData(GL_STATIC_DRAW, sizeof(Vertex) * vertices.size(), (void*)vertices.data());
		if (!mVBO)
		{
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "failed to create VBO");
		}

		mEBO = ElementBuffer::createWithData(GL_STATIC_DRAW, sizeof(uint32_t) * indices.size(), (void*)indices.data());
		if (!mEBO)
		{
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "failed to create EBO");
		}

		std::vector<VertexAttrib> attribs =
		{
			{ 3, GL_FLOAT, false, 0 },
			{ 2, GL_FLOAT, false, offsetof(Vertex, vTexcoord) },
			{ 3, GL_FLOAT, false, offsetof(Vertex, vNormal) },
			{ 3, GL_FLOAT, false, offsetof(Vertex, vTangent) },
			{ 3, GL_FLOAT, false, offsetof(Vertex, vBitangent) }
		};

		mVAO = VertexArray::createWithData(mVBO.get(), mEBO.get(), sizeof(Vertex), attribs);
		if (!mVAO)
		{
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "failed to create VAO");
		}
	}

	Mesh::~Mesh()
	{
		mVertices.swap(std::vector<Vertex>());
		mIndices.swap(std::vector<uint32_t>());

		mVAO.reset(nullptr);
		mVBO.reset(nullptr);
		mEBO.reset(nullptr);
	}

	template<typename... T>
	std::shared_ptr<Mesh> Mesh::createWithData(T &&... args)
	{
		return std::make_shared<Mesh>(std::forward<T>(args)...);
	}

	void Mesh::render()
	{
		mVAO->bind();
		glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
		mVAO->unbind();
	}
}