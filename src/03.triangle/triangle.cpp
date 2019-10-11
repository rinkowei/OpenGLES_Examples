#include <examplebase.h>
#include <mesh.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Mesh* triangle;

	Example()
	{
		title = "triangle";
		settings.vsync = true;
		defaultClearColor = glm::vec4(0.40f, 0.40f, 0.50f, 1.0f);

		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/03.triangle/";
	}
	~Example()
	{
		delete(triangle);
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		std::vector<GLfloat> vertexPositions = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f,  0.5f, 0.0f
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
			{ Material::ShaderType::VERTEX, shadersDirectory + "triangle.vert" },
			{ Material::ShaderType::FRAGMENT, shadersDirectory + "triangle.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> texturePaths = {};

		// create triangle material
		std::shared_ptr<Material> material = std::make_shared<Material>(shaderPaths, texturePaths);
		
		// create triangle mesh
		triangle = Mesh::createWithData(vertices, {}, material, Mesh::DrawType::ARRAYS);
	}

	virtual void render(float deltaTime) override
	{
		SDL_GetWindowSize(window, &destWidth, &destHeight);
		glViewport(0, 0, destWidth, destHeight);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT);

		triangle->render(deltaTime);
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


