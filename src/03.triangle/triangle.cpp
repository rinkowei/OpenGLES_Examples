﻿
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Mesh* triangle;
	Shader* shader;
	Material* material;
	Example()
	{
		title = "triangle";
		defaultClearColor = glm::vec4(0.40f, 0.40f, 0.50f, 1.0f);

		shadersPath = getResourcesPath(ResourceType::Shader) + "/03.triangle/";
	}
	~Example()
	{
		delete(shader);
		delete(triangle);
	}
public:
	virtual void prepare() override
	{
		std::vector<GLfloat> vertexPositions = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f,  0.5f, 0.0f
		};
		
		std::vector<Vertex> vertices = {};
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexPositions.size() / 3); i++)
		{
			Vertex vertex;
			vertex.Position = glm::vec3(vertexPositions[i * 3], vertexPositions[i * 3 + 1], vertexPositions[i * 3 + 2]);
			vertices.push_back(vertex);
		}
		
		// create triangle mesh
		triangle = Mesh::createWithData(vertices, {}, {}, Mesh::DrawType::Arrays);

		shader = Shader::createWithFile(shadersPath + "triangle.vs", shadersPath + "triangle.fs");

		std::unordered_map<Material::ShaderType, std::string> shaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersPath + "triangle.vs" },
			{ Material::ShaderType::Fragment, shadersPath + "triangle.fs" }
		};
		// create triangle material
		material = Material::createWithFile(shaderPaths);
	}
	virtual void render() override
	{
		// apply material for render triangle
		material->apply();
		triangle->Draw(shader);
	}
};

Example* example;
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	example = new Example();
	example->setupValidation();
	if (!example->setupGLFW() ||
		!example->loadGLESFunctions() ||
		!example->setupImGui())
	{
		return 0;
	}
	example->prepare();
	example->renderLoop();
	delete(example);
	return 0;
}

