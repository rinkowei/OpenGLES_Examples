
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Mesh* quadrangle;
	Shader* shader;
	Example()
	{
		title = "quadrangle indices";
		defaultClearColor = glm::vec4(0.40f, 0.40f, 0.50f, 1.0f);

		shadersPath = getResourcesPath(ResourceType::Shader) + "/04.quadrangle_indices/";
	}
	~Example()
	{
		delete(shader);
		delete(quadrangle);
	}
public:
	virtual void prepare() override
	{
		std::vector<GLfloat> vertexPositions = {
			 0.5f,  0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			-0.5f, -0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		std::vector<GLuint> indices = {
			0, 1, 3,
			1, 2, 3
		};

		std::vector<Vertex> vertices = {};
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexPositions.size() / 3); i++)
		{
			Vertex vertex;
			vertex.Position = glm::vec3(vertexPositions[i * 3], vertexPositions[i * 3 + 1], vertexPositions[i * 3 + 2]);
			vertices.push_back(vertex);
		}

		// create quadrangle mesh
		quadrangle = Mesh::createWithData(vertices, indices, {});

		// create triangle shader
		shader = Shader::createWithFile(shadersPath + "quadrangle.vs", shadersPath + "quadrangle.fs");
	}
	virtual void render() override
	{
		// render mesh
		quadrangle->Draw(shader);
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


