
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Mesh* cube1;
	Mesh* cube2;

	Mesh* outlineCube1;
	Mesh* outlineCube2;

	Example()
	{
		title = "object outlining";
		settings.vsync = false;
		defaultClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/12.object_outlining/";
		texturesDirectory = getResourcesPath(ResourceType::Texture) + "/12.object_outlining/";
	}
	~Example()
	{
		delete(cube1);
		delete(cube2);
		delete(outlineCube1);
		delete(outlineCube2);
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// setup camera
		camera->movementSpeed = 1.0f;
		camera->rotationSpeed = 0.5f;
		camera->setPosition(glm::vec3(0.0f, 0.0f, 3.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		// enable stencil test
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		std::vector<GLfloat> vertexAttrs = {
			// positions          // texture Coords
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

		std::unordered_map<Material::ShaderType, std::string> cubeShaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "cube.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "cube.frag" }
		};

		std::unordered_map<Material::ShaderType, std::string> outlineShaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "cube.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "outline_color.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> cubeTexturePaths =
		{
			std::make_pair(Texture::Type::Diffuse, texturesDirectory + "marble.jpg")
		};

		std::vector<std::pair<Texture::Type, std::string>> outlineTexturePaths =
		{

		};

		// create cube material
		std::shared_ptr<Material> cubeMat = std::make_shared<Material>(cubeShaderPaths, cubeTexturePaths);
		// create outline material
		std::shared_ptr<Material> outlineMat = std::make_shared<Material>(outlineShaderPaths, outlineTexturePaths);

		// create cube1 mesh
		cube1 = Mesh::createWithData(vertices, {}, Mesh::DrawType::Arrays, cubeMat);
		// create outline1 mesh
		outlineCube1 = Mesh::createWithData(vertices, {}, Mesh::DrawType::Arrays, outlineMat);
		outlineCube1->setScale(glm::vec3(1.1f, 1.1f, 1.1f));

		// create cube2 mesh
		cube2 = Mesh::createWithData(vertices, {}, Mesh::DrawType::Arrays, cubeMat);
		cube2->setPosition(glm::vec3(1.0f, 0.0f, -2.0f));
		// create outline2 mesh
		outlineCube2 = Mesh::createWithData(vertices, {}, Mesh::DrawType::Arrays, outlineMat);
		outlineCube2->setPosition(glm::vec3(1.0f, 0.0f, -2.0f));
		outlineCube2->setScale(glm::vec3(1.1f, 1.1f, 1.1f));
	}

	virtual void render(float deltaTime) override
	{
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		cube1->render(deltaTime);
		cube2->render(deltaTime);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		outlineCube1->render(deltaTime);
		outlineCube2->render(deltaTime);

		glStencilMask(0xFF);
		glEnable(GL_DEPTH_TEST);
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