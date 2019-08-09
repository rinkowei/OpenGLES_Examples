
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Mesh* triangle;
	Material* material;
	Example()
	{
		title = "triangle restart index";
		defaultClearColor = glm::vec4(0.40f, 0.40f, 0.50f, 1.0f);

		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/05.triangle_restart_index/";
	}
	~Example()
	{
		
	}
public:
	virtual void prepare() override
	{
		std::vector<GLfloat> vertexPositions = {
			-0.6f, -0.3f, 0.0f,
			-0.3f, 0.4f, 0.0f,
			0.0f, -0.3f, 0.0f,
			0.3f, 0.4f, 0.0f,
			0.6f, -0.3f, 0.0f
		};

		std::vector<GLuint> indices = {
			0, 1, 2,
			0xFFFF,    // restart index
			2, 3, 4
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
			{ Material::ShaderType::Vertex, shadersDirectory + "triangle.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "triangle.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> texturePaths = {};

		// create triangle material
		material = Material::createWithFile(shaderPaths, texturePaths);

		// create triangle mesh
		triangle = Mesh::createWithData(vertices, indices, Mesh::DrawType::Elements_Restart_Index, material);

		addObject(static_cast<Object*>(triangle));
	}
	virtual void update() override
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



