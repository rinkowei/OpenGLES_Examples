
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Mesh* quad;
	Shader* shader;
	Example()
	{
		title = "quad with texture";
		defaultClearColor = glm::vec4(0.2f, 0.2f, 0.4f, 1.0f);

		shadersPath = getResourcesPath(ResourceType::Shader) + "/07.quad_with_texture/";
		texturesPath = getResourcesPath(ResourceType::Texture) + "/07.quad_with_texture/";
	}
	~Example()
	{
		delete(shader);
		delete(quad);
	}
public:
	virtual void prepare() override
	{
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

		std::vector<Texture*> textures = {};
		Texture* texture = Texture::createWithFile(texturesPath + "timg.png", Texture::Type::Diffuse);
		textures.push_back(texture);

		quad = Mesh::createWithData(vertices, indices, textures, Mesh::DrawType::Elements);
		// create triangle shader
		shader = Shader::createWithFile(shadersPath + "quad.vs", shadersPath + "quad.fs");
	}
	virtual void render() override
	{
		quad->Draw(shader);
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