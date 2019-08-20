
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
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

		std::unordered_map<Material::ShaderType, std::string> shaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "quadrangle.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "quadrangle.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> texturePaths = {};

		// create quadrangle material
		std::shared_ptr<Material> material = std::make_shared<Material>(shaderPaths, texturePaths);

		// create quadrangle mesh
		Mesh* quadrangle = Mesh::createWithData(vertices, indices, Mesh::DrawType::Elements, material);

		addObject(static_cast<Object*>(quadrangle));
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


