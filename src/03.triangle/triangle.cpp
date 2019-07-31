
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Mesh* triangle;
	Material* material;
	Example()
	{
		title = "triangle";
		defaultClearColor = glm::vec4(0.40f, 0.40f, 0.50f, 1.0f);

		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/03.triangle/";
	}
	~Example()
	{
		delete(material);
		delete(triangle);
	}
public:
	virtual void prepare() override
	{
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
		
		// create triangle mesh
		triangle = Mesh::createWithData(vertices, {}, {}, Mesh::DrawType::Arrays);

		std::unordered_map<Material::ShaderType, std::string> shaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "triangle.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "triangle.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> texturePaths = {};
		
		// create triangle material
		material = Material::createWithFile(shaderPaths, texturePaths);
	}
	virtual void render() override
	{
		// apply material for render triangle
		material->apply();
		triangle->Draw();
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


