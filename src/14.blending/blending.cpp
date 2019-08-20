
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Example()
	{
		title = "blending";
		settings.vsync = false;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/14.blending/";
	}
	~Example()
	{

	}
public:
	virtual void prepare() override
	{
		// setup camera
		camera->movementSpeed = 1.0f;
		camera->rotationSpeed = 0.5f;
		camera->setPosition(glm::vec3(0.0f, 0.0f, 4.0f));

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
			float distance = glm::length(camera->getPosition() - quadPositions[i]);
			sortedMap[distance] = quadPositions[i];
		}

		std::vector<GLfloat> vertexAttrs = {
			// positions         // texture coordinates
			0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
			0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
			1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

			0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
			1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
			1.0f,  0.5f,  0.0f,  1.0f,  0.0f
		};

		std::vector<Vertex> vertices = {};
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexAttrs.size() / 8); i++)
		{
			Vertex vertex;
			vertex.Position = glm::vec3(vertexAttrs[i * 5], vertexAttrs[i * 5 + 1], vertexAttrs[i * 5 + 2]);
			vertex.TexCoords = glm::vec2(vertexAttrs[i * 5 + 3], vertexAttrs[i * 5 + 4]);
			vertices.push_back(vertex);
		}

		std::unordered_map<Material::ShaderType, std::string> shaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "quad.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "quad.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> texturePaths =
		{

		};

		// create quad material
		std::shared_ptr<Material> material = std::make_shared<Material>(shaderPaths, texturePaths);

		for (auto iter = sortedMap.rbegin(); iter != sortedMap.rend(); iter++)
		{
			Mesh* quad = Mesh::createWithData(vertices, {}, Mesh::DrawType::Arrays, material);
			quad->setPosition(iter->second);
			addObject(static_cast<Object*>(quad));
		}
	}

	virtual void update(float deltaTime) override
	{

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