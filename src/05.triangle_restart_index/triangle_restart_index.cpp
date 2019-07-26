
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Mesh* triangle;
	Shader* shader;
	Example()
	{
		title = "triangle restart index";
		defaultClearColor = glm::vec4(0.40f, 0.40f, 0.50f, 1.0f);

		shadersPath = getResourcesPath(ResourceType::Shader) + "/05.triangle_restart_index/";
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
			-0.6f, -0.3f, 0.0f,
			-0.3f, 0.4f, 0.0f,
			0.0f, -0.3f, 0.0f,
			0.3f, 0.4f, 0.0f,
			0.6f, -0.3f, 0.0f
		};

		std::vector<GLuint> indices = {
			0, 1, 2,
			0xFFFF,    // restart index
			2, 3, 4
		};

		std::vector<Vertex> vertices = {};
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexPositions.size() / 3); i++)
		{
			Vertex vertex;
			vertex.Position = glm::vec3(vertexPositions[i * 3], vertexPositions[i * 3 + 1], vertexPositions[i * 3 + 2]);
			vertices.push_back(vertex);
		}

		// create triangle mesh
		triangle = Mesh::createWithData(vertices, indices, {}, Mesh::DrawType::Elements_Restart_Index);

		// create triangle shader
		shader = Shader::createWithFile(shadersPath + "triangle.vs", shadersPath + "triangle.fs");
	}
	virtual void render() override
	{
		// render mesh
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



