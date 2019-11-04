#include <examplebase.h>
#include <mesh.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Mesh> triangle;

	Example()
	{
		title = "triangle restart index";
		settings.vsync = true;
		defaultClearColor = glm::vec4(0.40f, 0.40f, 0.50f, 1.0f);

		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/05.triangle_restart_index/";
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
			-0.5f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.0f
		}; 

		std::vector<Vertex> vertices = {};
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexAttribs.size() / 3); i++)
		{
			Vertex vertex;
			vertex.vPosition = glm::vec3(vertexAttribs[i * 3], vertexAttribs[i * 3 + 1], vertexAttribs[i * 3 + 2]);
			vertices.push_back(vertex);
		}

		std::vector<GLuint> indices = {
			0, 1, 2,
			0xFFFF,    // restart index
			1, 3, 2
		};

		std::shared_ptr<Material> mat = Material::createFromFiles("triangle_mat",
			{
				shadersDirectory + "triangle.vert",
				shadersDirectory + "triangle.frag"
			},
			{

			}
		);

		// create triangle mesh
		triangle = Mesh::createWithData("triangle", vertices, indices);
		triangle->setDrawType(Mesh::DrawType::ELEMENTS_RESTART_INDEX);
		triangle->setMaterial(mat);
	}

	virtual void render(float deltaTime) override
	{
		triangle->render();
	}

	virtual void windowResized() override
	{
		ExampleBase::windowResized();
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



