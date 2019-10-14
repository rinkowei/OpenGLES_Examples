#include <examplebase.h>
#include <mesh.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Mesh> triangle;
	std::unique_ptr<Program> program;

	Example()
	{
		title = "triangle";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.40f, 0.40f, 0.50f, 1.0f);

		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/03.triangle/";
	}
	~Example()
	{
		
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		std::vector<float> vertexAttribs = {
			// position
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f,  0.5f, 0.0f
		};
		
		std::vector<Vertex> vertices = {};
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexAttribs.size() / 3); i++)
		{
			Vertex vertex;
			vertex.vPosition = glm::vec3(vertexAttribs[i * 3], vertexAttribs[i * 3 + 1], vertexAttribs[i * 3 + 2]);
			vertices.push_back(vertex);
		}

		std::vector<uint32_t> indices = {};
		
		// create triangle mesh
		triangle = Mesh::createWithData("triangle", vertices, indices);
		triangle->setDrawType(Mesh::DrawType::ARRAYS);

		program = Program::createFromFiles(
			{
				shadersDirectory + "triangle.vert",
				shadersDirectory + "triangle.frag"
			}
		);
	}

	virtual void render(float deltaTime) override
	{
		SDL_GetWindowSize(window, &destWidth, &destHeight);
		glViewport(0, 0, destWidth, destHeight);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT);

		program->apply();
		triangle->render();
	}
};

Example* example;
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	example = new Example();
	example->setupValidation();
	if (!example->setupSDL() ||
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


