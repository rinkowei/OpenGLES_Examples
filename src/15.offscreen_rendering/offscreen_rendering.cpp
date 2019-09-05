
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Model* model;
	Mesh* offscreenQuad;
	GLuint framebuffer, colorAttachment, renderbuffer;

	Example()
	{
		title = "offscreen rendering";
		settings.vsync = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/15.offscreen_rendering/";
	}
	~Example()
	{
		delete(model);
		delete(offscreenQuad);
		glDeleteFramebuffers(1, &framebuffer);
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// setup camera
		camera->movementSpeed = 20.0f;
		camera->rotationSpeed = 0.5f;
		camera->setPosition(glm::vec3(0.0f, 0.0f, 4.0f));
		//camera->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		std::vector<GLfloat> vertexAttrs = {
			 // positions       // texture coordinates
			 0.4f, 1.0f, 0.0f,  1.0f, 1.0f,
			 0.4f, 0.2f, 0.0f,  1.0f, 0.0f,
			-0.4f, 0.2f, 0.0f,  0.0f, 0.0f,
			-0.4f, 1.0f, 0.0f,  0.0f, 1.0f
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
			{ Material::ShaderType::VERTEX, shadersDirectory + "screen.vert" },
			{ Material::ShaderType::FRAGMENT, shadersDirectory + "screen.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> texturePaths =
		{

		};

		// create offscreenQuad material
		std::shared_ptr<Material> material = std::make_shared<Material>(shaderPaths, texturePaths);

		// create offscreenQuad mesh
		offscreenQuad = Mesh::createWithData(vertices, indices, material, Mesh::DrawType::ELEMENTS);
		
		model = Model::createWithFile(modelsDirectory + "/construction-site-rawscan/site.obj", 
		{
			{ Material::ShaderType::VERTEX, shadersDirectory + "construction.vert" },
			{ Material::ShaderType::FRAGMENT, shadersDirectory + "construction.frag" }
		});

		// configure framebuffer
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		// attach one color buffer for write pixels data
		glGenTextures(1, &colorAttachment);
		glBindTexture(GL_TEXTURE_2D, colorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);

		// attach renderbuffer for write depth value and stencil valie
		glGenRenderbuffers(1, &renderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
	}

	virtual void render(float deltaTime) override
	{
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		// clear color and depth buffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		model->render(deltaTime);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		model->render(deltaTime);

		// diable depth test for render quad in front of scene
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorAttachment);
		offscreenQuad->getMaterial()->setInteger("screenTexture", 0);
		offscreenQuad->render(deltaTime);
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