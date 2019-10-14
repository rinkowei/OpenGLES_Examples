#include "mesh.h"

namespace es
{
	Mesh::Mesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) : Object(name)
	{
		mName = name;
		mVertices = vertices;
		mIndices = indices;

		mDrawType = DrawType::ELEMENTS;

		mVBO = VertexBuffer::createWithData(GL_STATIC_DRAW, sizeof(Vertex) * vertices.size(), (void*)vertices.data());
		if (!mVBO)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "failed to create VBO");
		}

		mEBO = ElementBuffer::createWithData(GL_STATIC_DRAW, sizeof(uint32_t) * indices.size(), (void*)indices.data());
		if (!mEBO)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "failed to create EBO");
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
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "failed to create VAO");
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

	void Mesh::setMaterial(std::shared_ptr<Material> mMat)
	{
		if (mMaterial != nullptr)
		{
			mMaterial.reset();
		}
	    mMaterial = mMat;
	}

	void Mesh::setDrawType(DrawType drawType)
	{
		mDrawType = drawType;
	}

	void Mesh::setInstancingData(uint64_t size, void* data, uint32_t count)
	{
		if (mIBO.has_value())
		{
			mIBO.value().reset(nullptr);
		}
		mIBO = InstanceBuffer::createWithData(GL_STATIC_DRAW, size, data);
		mInstanceCount = count;

		mVAO->bind();
		mIBO.value()->bind();

		glVertexAttribPointer(mVAO->getVertexAttribCount(), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(mVAO->getVertexAttribCount());
		glVertexAttribDivisor(mVAO->getVertexAttribCount(), 1);

		mIBO.value()->unbind();
		mVAO->bind();
	}

	void Mesh::render()
	{
		mVAO->bind();

		switch (mDrawType)
		{
			case DrawType::ARRAYS:
			{
				GLES_CHECK_ERROR(glDrawArrays(GL_TRIANGLES, 0, mVertices.size()));
				break;
			}
			case DrawType::ARRAYS_INDIRECT:
			{

			}
			case DrawType::ARRAYS_INSTANCED:
			{

			}
			case DrawType::ELEMENTS:
			{
				GLES_CHECK_ERROR(glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0));
				break;
			}
			case DrawType::ELEMENTS_INDIRECT:
			{

			}
			case DrawType::ELEMENTS_INSTANCED:
			{
				GLES_CHECK_ERROR(glDrawElementsInstanced(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0, mInstanceCount.value()));
				break;
			}
			case DrawType::ELEMENTS_RESTART_INDEX:
			{
				GLES_CHECK_ERROR(glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX));
				GLES_CHECK_ERROR(glDrawElements(GL_TRIANGLE_STRIP, mIndices.size(), GL_UNSIGNED_INT, 0));
				GLES_CHECK_ERROR(glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX));
				break;
			}
		}

		mVAO->unbind();
	}
}