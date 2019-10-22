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
	// unique program uniform in every single mesh
	struct ProgramUniform
	{
		std::string typeName;

		union UniformValue
		{
			int intValue;
			bool boolValue;
			float floatValue;
			glm::vec2 vec2Value;
			glm::vec3 vec3Value;
			glm::vec4 vec4Value;
			glm::mat2 mat2Value;
			glm::mat3 mat3Value;
			glm::mat4 mat4Value;
			/*
			std::vector<int> intVecValue;
			std::vector<bool> boolVecValue;
			std::vector<float> floatVecValue;
			std::vector<glm::vec2> vec2VecValue;
			std::vector<glm::vec3> vec3VecValue;
			std::vector<glm::vec4> vec4VecValue;
			std::vector<glm::mat2> mat2VecValue;
			std::vector<glm::mat3> mat3VecValue;
			std::vector<glm::mat4> mat4VecValue;
			*/
		} uniformValue;
	};

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
		Mesh(const std::string& name, const Mesh* mesh);
		~Mesh();

		static std::shared_ptr<Mesh> createWithData(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
	
		static std::shared_ptr<Mesh> clone(const std::string& name, const Mesh* mesh);

		void setMaterial(std::shared_ptr<Material> mMat);

		void setDrawType(DrawType drawType);

		template<typename T>
		void setInstancingData(uint64_t size, void* data, uint32_t count)
		{
			if (mIBO.has_value())
			{
				mIBO.value().reset();
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

		void render(bool isUseLocalMaterial = true);

		virtual void update() override;

		std::shared_ptr<Material> getMaterial() const;

		template<typename T>
		void setUniform(const std::string& name, const T& value)
		{
			ProgramUniform proUni;
			proUni.typeName = typeid(T).name();
		
			if (proUni.typeName == "float")
			{
				proUni.uniformValue.floatValue = value;
			}
			mProgramUniformMap.insert(std::make_pair(name, proUni));
		}
	private:
		std::vector<Vertex> mVertices;
		std::vector<uint32_t> mIndices;

		// vertex array object
		std::shared_ptr<VertexArray> mVAO = nullptr;
		// vertex buffer object
		std::shared_ptr<VertexBuffer> mVBO = nullptr;
		// element buffer object
		std::shared_ptr<ElementBuffer> mEBO = nullptr;
		// instance buffer object
		std::optional<std::shared_ptr<InstanceBuffer>> mIBO = std::nullopt;
		std::optional<uint32_t> mInstanceCount = std::nullopt;
		
		std::shared_ptr<Material> mMaterial = nullptr;

		std::unordered_map<std::string, ProgramUniform> mProgramUniformMap;

		DrawType mDrawType;
	};
}

#endif