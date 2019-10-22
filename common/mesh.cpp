#include "mesh.h"

namespace es
{
	Mesh::Mesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) : Object(name)
	{
		mVertices.assign(vertices.begin(), vertices.end());
		mIndices.assign(indices.begin(), indices.end());

		mDrawType = DrawType::ELEMENTS;

		mVBO = VertexBuffer::createWithData(GL_STATIC_DRAW, sizeof(Vertex) * mVertices.size(), (void*)mVertices.data());
		if (!mVBO)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "failed to create VBO");
		}

		mEBO = ElementBuffer::createWithData(GL_STATIC_DRAW, sizeof(uint32_t) * mIndices.size(), (void*)mIndices.data());
		if (!mEBO)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "failed to create EBO");
		}

		std::vector<VertexAttrib> attribLayout = {};

		if (mVertices[0].vPosition.has_value())
		{
			attribLayout.push_back({ 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, vPosition) });
		}
		if (mVertices[0].vTexcoord.has_value())
		{
			attribLayout.push_back({ 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, vTexcoord) });
		}
		if (mVertices[0].vNormal.has_value())
		{
			attribLayout.push_back({ 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, vNormal) });
		}
		if (mVertices[0].vTangent.has_value())
		{
			attribLayout.push_back({ 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, vTangent) });
		}
		if (mVertices[0].vBitangent.has_value())
		{
			attribLayout.push_back({ 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, vBitangent) });
		}
		if (mVertices[0].vColor.has_value())
		{
			attribLayout.push_back({ 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, vColor) });
		}

		mVAO = VertexArray::createWithData(mVBO.get(), mEBO.get(), sizeof(Vertex), attribLayout);
		if (!mVAO)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "failed to create VAO");
		}
	}

	Mesh::Mesh(const std::string& name, const Mesh* mesh) : Object(name)
	{
		mPosition = mesh->mPosition;
		mRotation = mesh->mRotation;
		mScaling = mesh->mScaling;
		mModelMatrix = mesh->mModelMatrix;
		mAutoUpdated = mesh->mAutoUpdated;
		mTransformUpdated = mesh->mTransformUpdated;
		mIsDirty = mesh->mIsDirty;

		mVertices.assign(mesh->mVertices.begin(), mesh->mVertices.end());
		mIndices.assign(mesh->mIndices.begin(), mesh->mIndices.end());

		mVAO = mesh->mVAO;
		mVBO = mesh->mVBO;
		mEBO = mesh->mEBO;
		mIBO = mesh->mIBO;
		mInstanceCount = mesh->mInstanceCount;

		mMaterial = mesh->mMaterial;

		mDrawType = mesh->mDrawType;
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

		mVAO.reset();
		mVAO = nullptr;
		mVBO.reset();
		mVBO = nullptr;
		mEBO.reset();
		mEBO = nullptr;
	}

	std::shared_ptr<Mesh> Mesh::createWithData(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
	{
		return std::make_shared<Mesh>(name, vertices, indices);
	}

	std::shared_ptr<Mesh> Mesh::clone(const std::string& name, const Mesh* mesh)
	{
		return std::make_shared<Mesh>(name, mesh);
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
			
			for (auto iter = mProgramUniformMap.begin(); iter != mProgramUniformMap.end(); iter++)
			{
				if (iter->second.typeName == "int")
				{
					mMaterial->setUniform(iter->first, iter->second.uniformValue.intValue);
				}
				else if (iter->second.typeName == "float")
				{
					mMaterial->setUniform(iter->first, iter->second.uniformValue.floatValue);
				}
			}
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