
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Mesh* cube;
	Material* material;
	Example()
	{
		title = "transformations";
		defaultClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/09.transformations/";
		texturesDirectory = getResourcesPath(ResourceType::Texture) + "/09.transformations/";
	}
	~Example()
	{
		delete(cube);
	}
public:
	virtual void prepare() override
	{
		// setup camera
		camera->type = Camera::Type::firstPerson;
		camera->rotationSpeed = 0.5f;
		camera->setPosition(glm::vec3(0.0f, 0.0f, -4.0f));

		glEnable(GL_DEPTH_TEST);

		std::vector<GLfloat> vertexAttrs = {
			// positions         // texture coordinates
		   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		   -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		   -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		   -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		   -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		   -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
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
			{ Material::ShaderType::Vertex, shadersDirectory + "cube.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "cube.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> texturePaths =
		{
			std::make_pair(Texture::Type::Diffuse, texturesDirectory + "face.png")
		};

		// create quad material
		material = Material::createWithFile(shaderPaths, texturePaths);

		cube = Mesh::createWithData(vertices, {}, Mesh::DrawType::Arrays, material);
	}
	virtual void render() override
	{
		if (!paused || camera->updated)
		{
			// initialize matrix to identity matrix
			glm::mat4 model = glm::mat4(1.0f);

			// scale
			float scale = glm::max(glm::sin((double)timePassed * 2.0), 0.3);
			model = glm::scale(model, glm::vec3(scale));

			// translation
			model = glm::translate(model, glm::vec3(0.1f, 0.4f, 0.3f));

			// rotation
			model = glm::rotate(model, timePassed * 2.0f * glm::radians(90.0f), glm::vec3(0.5f, 0.2f, 0.8f));

			//material->setMat4("model", model);
			//material->setMat4("view", camera.matrices.view);
			//material->setMat4("projection", camera.matrices.projection);
		}

		// render quad mesh
		cube->render();
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