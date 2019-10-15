#include <examplebase.h>
#include <mesh.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Mesh> quad;

	Example()
	{
		title = "quad with texture";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.1f, 0.1f, 0.7f, 1.0f);

		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/07.quad_with_texture/";
		texturesDirectory = getResourcesPath(ResourceType::Texture) + "/07.quad_with_texture/";
	}
	~Example()
	{
		
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		std::vector<float> vertexAttribs = {
			// positions          // texture coordinates
			0.5f,  0.5f, 0.0f,    1.0f, 1.0f,
			0.5f, -0.5f, 0.0f,    1.0f, 0.0f,
		   -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,
		   -0.5f,  0.5f, 0.0f,    0.0f, 1.0f
		};

		std::vector<uint32_t> indices = {
			0, 1, 3,
			1, 2, 3
		};

		std::vector<Vertex> vertices = {};
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexAttribs.size() / 5); i++)
		{
			Vertex vertex;
			vertex.vPosition = glm::vec3(vertexAttribs[i * 5], vertexAttribs[i * 5 + 1], vertexAttribs[i * 5 + 2]);
			vertex.vTexcoord = glm::vec2(vertexAttribs[i * 5 + 3], vertexAttribs[i * 5 + 4]);
			vertices.push_back(vertex);
		}

		std::shared_ptr<Material> mat = Material::createFromFiles("quad_mat",
			{
				shadersDirectory + "quad.vert",
				shadersDirectory + "quad.frag"
			},
			{
				{ "diffuseMap_0", texturesDirectory + "timg.png" }
			}
		);

		quad = Mesh::createWithData("quad", vertices, indices);
		quad->setDrawType(Mesh::DrawType::ELEMENTS);
		quad->setMaterial(mat);
	}

	virtual void render(float deltaTime) override
	{
		SDL_GetWindowSize(window, &destWidth, &destHeight);
		glViewport(0, 0, destWidth, destHeight);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT);

		quad->render();
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