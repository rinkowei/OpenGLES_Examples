#ifndef MESH_H_
#define MESH_H_

#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <optional>

#include <buffer.h>
#include <material.h>
#include <world.h>

using namespace std;

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

	private:
		std::vector<Vertex*> mVertices;
		std::vector<uint32_t> mIndices;

		std::unique_ptr<VertexArray> mVAO = nullptr;
		std::unique_ptr<VertexBuffer> mVBO = nullptr;
		std::unique_ptr<ElementBuffer> mEBO = nullptr;
	};
}

#endif