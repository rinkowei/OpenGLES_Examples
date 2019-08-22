
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Example()
	{
		title = "quad with texture";
		settings.vsync = true;
		defaultClearColor = glm::vec4(0.1f, 0.1f, 0.2f, 1.0f);

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

		std::vector<GLfloat> vertexAttrs = {
			// positions          // texture coordinates
			0.5f,  0.5f, 0.0f,    1.0f, 1.0f,
			0.5f, -0.5f, 0.0f,    1.0f, 0.0f,
		   -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,
		   -0.5f,  0.5f, 0.0f,    0.0f, 1.0f
		};

		std::vector<GLuint> indices = {
			0, 1, 3,
			1, 2, 3
		};

		std::vector<Vertex> vertices = {};
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexAttrs.size() / 5); i++)
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
			std::make_pair(Texture::Type::Diffuse, texturesDirectory + "timg.png")
		};

		// create quad material
		std::shared_ptr<Material> material = std::make_shared<Material>(shaderPaths, texturePaths);

		// create quad mesh
		Mesh* quad = Mesh::createWithData(vertices, indices, Mesh::DrawType::Elements, material);

		addObject(static_cast<Object*>(quad));
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