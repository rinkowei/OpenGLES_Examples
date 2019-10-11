#ifndef MESH_H_
#define MESH_H_

#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <buffer.h>

namespace es
{
	// vertex attribute structure
	struct Vertex
	{
		glm::vec3 vPosition;
		glm::vec2 vTexcoord;
		glm::vec3 vNormal;
		glm::vec3 vTangent;
		glm::vec3 vBitangent;
	};

	class Mesh
	{
	public:
		~Mesh();

		template<typename... T>
		static std::shared_ptr<Mesh> createWithData(T &&... args)
		{
			struct EnableMakeShared : public Mesh
			{
				EnableMakeShared(T&&... args) : Mesh(std::forward<T>(args)...) {}
			};

			return std::static_pointer_cast<Mesh>(std::make_shared<EnableMakeShared>(std::forward<T>(args)...));
		}

		void render();
	protected:
		Mesh(const Mesh&) = delete;
		const Mesh& operator=(const Mesh&) = delete;
	private:
		Mesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

		std::string mName;

		std::vector<Vertex> mVertices;
		std::vector<uint32_t> mIndices;

		std::unique_ptr<VertexArray> mVAO = nullptr;
		std::unique_ptr<VertexBuffer> mVBO = nullptr;
		std::unique_ptr<ElementBuffer> mEBO = nullptr;
	};
}

#endif