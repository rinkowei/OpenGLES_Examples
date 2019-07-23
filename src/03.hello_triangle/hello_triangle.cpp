
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Mesh* triangle;
	Shader* shader;
	std::vector<Vertex> vertices = {};
	Example()
	{
		title = "hello triangle";
		settings.validation = true;
		defaultClearColor = glm::vec4(0.40f, 0.40f, 0.50f, 1.0f);
	}
	~Example()
	{
		delete(shader);
		delete(triangle);
	}
public:
	virtual void prepare() override final
	{
		std::vector<GLfloat> vertexPositions = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f,  0.5f, 0.0f
		};
		
		for (unsigned int i = 0; i < 3; i++)
		{
			Vertex vertex;
			vertex.Position = glm::vec3(vertexPositions[i * 3], vertexPositions[i * 3 + 1], vertexPositions[i * 3 + 2]);
			vertices.push_back(vertex);
		}
		
		triangle = Mesh::createWithData(vertices, {}, {});

		shader = Shader::createWithFile(getResourcesPath() + "shaders/3.hello_triangle/triangle.vs", getResourcesPath() + "shaders/3.hello_triangle/triangle.fs");
	}
	virtual void render() override final
	{
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


