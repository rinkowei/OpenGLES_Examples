#ifndef MESH_H_
#define MESH_H_

#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <optional>

#include <object.h>
#include <buffer.h>
#include <material.h>

namespace es
{
	// vertex attribute structure
	struct Vertex
	{
		std::optional<glm::vec3> vPosition;
		std::optional<glm::vec2> vTexcoord;
		std::optional<glm::vec3> vNormal;
		std::optional<glm::vec3> vTangent;
		std::optional<glm::vec3> vBitangent;
		std::optional<glm::vec4> vColor;
	};

	class Mesh : public Object
	{
	public:
		enum class DrawType
		{
			ARRAYS,
			ARRAYS_INDIRECT,
			ARRAYS_INSTANCED,
			ELEMENTS,
			ELEMENTS_INDIRECT,
			ELEMENTS_INSTANCED,
			ELEMENTS_RESTART_INDEX
		};

		Mesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		~Mesh();

		static std::shared_ptr<Mesh> createWithData(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
	
		void setMaterial(std::shared_ptr<Material> mMat);

		void setDrawType(DrawType drawType);

		template<typename T>
		void setInstancingData(uint64_t size, void* data, uint32_t count)
		{
			if (mIBO.has_value())
			{
				mIBO.value().reset(nullptr);
			}
			mIBO = InstanceBuffer::createWithData(GL_STATIC_DRAW, size, data);
			mInstanceCount = count;

			mVAO->bind();
			mIBO.value()->bind();

			glVertexAttribPointer(mVAO->getVertexAttribCount(), size / count / sizeof(T), GL_FLOAT, GL_FALSE, size / count, (void*)0);
			glEnableVertexAttribArray(mVAO->getVertexAttribCount());
			glVertexAttribDivisor(mVAO->getVertexAttribCount(), 1);

			mIBO.value()->unbind();
			mVAO->bind();
		}

		virtual void render() override;

		virtual void update() override;

		std::shared_ptr<Material> getMaterial() const;
	private:
		std::string mName;

		std::vector<Vertex> mVertices;
		std::vector<uint32_t> mIndices;

		// vertex array object
		std::unique_ptr<VertexArray> mVAO = nullptr;
		// vertex buffer object
		std::unique_ptr<VertexBuffer> mVBO = nullptr;
		// element buffer object
		std::unique_ptr<ElementBuffer> mEBO = nullptr;
		// instance buffer object
		std::optional<std::unique_ptr<InstanceBuffer>> mIBO = std::nullopt;
		std::optional<uint32_t> mInstanceCount = std::nullopt;
		
		std::shared_ptr<Material> mMaterial = nullptr;

		DrawType mDrawType;
	};
}

#endif