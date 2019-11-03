#include <examplebase.h>
#include <mesh.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Mesh> quadrangle;

	Example()
	{
		title = "quadrangle indices";
		settings.vsync = true;
		defaultClearColor = glm::vec4(0.40f, 0.40f, 0.50f, 1.0f);

		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/04.quadrangle_indices/";
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
			 0.5f,  0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			-0.5f, -0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		std::vector<Vertex> vertices = {};
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexAttribs.size() / 3); i++)
		{
			Vertex vertex;
			vertex.vPosition = glm::vec3(vertexAttribs[i * 3], vertexAttribs[i * 3 + 1], vertexAttribs[i * 3 + 2]);
			vertices.push_back(vertex);
		}

		std::vector<GLuint> indices = {
			0, 1, 3,
			1, 2, 3
		};

		std::shared_ptr<Material> mat = Material::createFromFiles("quadrangle_mat",
			{
				shadersDirectory + "quadrangle.vert",
				shadersDirectory + "quadrangle.frag"
			},
			{

			}
		);

		// create quadrangle mesh
		quadrangle = Mesh::createWithData("quadrangle", vertices, indices);
		quadrangle->setDrawType(Mesh::DrawType::ELEMENTS);
		quadrangle->setMaterial(mat);
	}

	virtual void render(float deltaTime) override
	{
		SDL_GetWindowSize(window, &windowWidth, &windowHeight);
		glViewport(0, 0, windowWidth, windowHeight);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT);

		quadrangle->render();
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


