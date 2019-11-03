#include <examplebase.h>
#include <mesh.h>
#include <material.h>
#include <map>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::vector<std::shared_ptr<Mesh>> quads;

	Example()
	{
		title = "blending";
		settings.vsync = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/14.blending/";
	}
	~Example()
	{

	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		// enable blend
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);

		// all positions of quads
		std::array<glm::vec3, 6> quadPositions
		{
			glm::vec3(-1.5f, 0.0f, -0.48f),
			glm::vec3(1.5f, 0.0f, 0.51f),
			glm::vec3(0.0f, 0.0f, 0.7f),
			glm::vec3(0.5f, 0.0f, -0.6f),
			glm::vec3(0.2f, 0.0f, -0.9f),
			glm::vec3(-0.2f, 0.0f, -1.4f)
		};

		std::map<float, glm::vec3> sortedMap;
		for (unsigned int i = 0; i < quadPositions.size(); i++)
		{
			float distance = glm::length(mMainCamera->getPosition() - quadPositions[i]);
			sortedMap[distance] = quadPositions[i];
		}

		std::vector<float> vertexAttribs = {
			// positions         // texture coordinates
			0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
			0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
			1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

			0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
			1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
			1.0f,  0.5f,  0.0f,  1.0f,  0.0f
		};

		std::vector<Vertex> vertices = {};
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexAttribs.size() / 8); i++)
		{
			Vertex vertex;
			vertex.vPosition = glm::vec3(vertexAttribs[i * 5], vertexAttribs[i * 5 + 1], vertexAttribs[i * 5 + 2]);
			vertex.vTexcoord = glm::vec2(vertexAttribs[i * 5 + 3], vertexAttribs[i * 5 + 4]);
			vertices.push_back(vertex);
		}

		std::shared_ptr<Material> mat = Material::createFromFiles("mat",
			{
				shadersDirectory + "quad.vert",
				shadersDirectory + "quad.frag"
			},
			{

			}
		);

		std::shared_ptr<Mesh> quadTemplate = Mesh::createWithData("quad_template", vertices, {});
		quadTemplate->setDrawType(Mesh::DrawType::ARRAYS);
		quadTemplate->setMaterial(mat);

		uint8_t i = 0;
		for (auto iter = sortedMap.rbegin(); iter != sortedMap.rend(); iter++)
		{
			std::shared_ptr<Mesh> quad = Mesh::clone("quad_" + std::to_string(i++), quadTemplate.get());
			quad->setPosition(iter->second);
			quads.push_back(quad);
		}
	}

	virtual void render(float deltaTime) override
	{
		SDL_GetWindowSize(window, &windowWidth, &windowHeight);
		glViewport(0, 0, windowWidth, windowHeight);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		for (std::size_t i = 0; i < quads.size(); i++)
		{
			quads[i]->render();
		}
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