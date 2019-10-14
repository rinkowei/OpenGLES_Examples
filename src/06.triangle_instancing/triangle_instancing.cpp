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
		title = "triangle instancing";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/06.triangle_instancing/";
	}
	~Example()
	{

	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// create a array of 100 triangle locations
		std::array<glm::vec2, 100> locations;
		unsigned int index = 0;
		float offset = 0.1f;
		for (int x = -10; x < 10; x += 2)
		{
			for (int y = -10; y < 10; y += 2)
			{
				glm::vec2 position((float)x / 10.0f + offset, (float)y / 10.0f + offset);
				locations[index++] = position;
			}
		}

		std::vector<float> vertexAttribs = {
			 // positions           // colors
			 0.05f, -0.05f, 0.0f,   1.0f, 0.0f, 0.0f, 1.0f,
			-0.05f, -0.05f, 0.0f,   0.0f, 1.0f, 0.0f, 1.0f,
			-0.05f,  0.05f, 0.0f,   0.0f, 0.0f, 1.0f, 1.0f,
		};

		std::vector<uint32_t> indices = {
			0, 1, 2
		};

		std::vector<Vertex> vertices = {};
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexAttribs.size() / 7); i++)
		{
			Vertex vertex;
			vertex.vPosition = glm::vec3(vertexAttribs[i * 7], vertexAttribs[i * 7 + 1], vertexAttribs[i * 7 + 2]);
			vertex.vColor = glm::vec4(vertexAttribs[i * 7 + 3], vertexAttribs[i * 7 + 4], vertexAttribs[i * 7 + 5], vertexAttribs[i * 7 + 6]);
			vertices.push_back(vertex);
		}

		// create triangle mesh
		triangle = Mesh::createWithData("triangle", vertices, indices);
		triangle->setDrawType(Mesh::DrawType::ELEMENTS_INSTANCED);
		triangle->setInstancingData<float>(sizeof(glm::vec2) * locations.size(), (void*)locations.data(), 100);

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




