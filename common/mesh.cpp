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

		std::vector<VertexAttrib> attribLayout = {};

		if (vertices[0].vPosition.has_value())
		{
			attribLayout.push_back({ 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, vPosition) });
		}
		if (vertices[0].vTexcoord.has_value())
		{
			attribLayout.push_back({ 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, vTexcoord) });
		}
		if (vertices[0].vNormal.has_value())
		{
			attribLayout.push_back({ 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, vNormal) });
		}
		if (vertices[0].vTangent.has_value())
		{
			attribLayout.push_back({ 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, vTangent) });
		}
		if (vertices[0].vBitangent.has_value())
		{
			attribLayout.push_back({ 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, vBitangent) });
		}
		if (vertices[0].vColor.has_value())
		{
			attribLayout.push_back({ 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, vColor) });
		}

		mVAO = VertexArray::createWithData(mVBO.get(), mEBO.get(), sizeof(Vertex), attribLayout);
		if (!mVAO)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "failed to create VAO");
		}
	}

	Mesh::~Mesh()
	{
		if (mMaterial != nullptr)
		{
			mMaterial.reset();
			mMaterial = nullptr;
		}

		mVertices.swap(std::vector<Vertex>());
		mIndices.swap(std::vector<uint32_t>());

		mVAO.reset(nullptr);
		mVBO.reset(nullptr);
		mEBO.reset(nullptr);
	}

	std::shared_ptr<Mesh> Mesh::createWithData(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
	{
		return std::make_shared<Mesh>(name, vertices, indices);
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

	void Mesh::render(bool isUseLocalMaterial)
	{
		if (mAutoUpdated)
		{
			update();
		}

		if (isUseLocalMaterial && mMaterial != nullptr)
		{
			mMaterial->apply();
			mMaterial->setUniform("model", mModelMatrix);
			mMaterial->setUniform("view", camera->getView());
			mMaterial->setUniform("projection", camera->getProjection());
		}
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
		if (isUseLocalMaterial && mMaterial != nullptr)
		{
			mMaterial->unapply();
		}
	}

	void Mesh::update()
	{
		Object::update();
	}

	std::shared_ptr<Material> Mesh::getMaterial() const
	{
		return mMaterial;
	}
}