#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> model;
	std::shared_ptr<Mesh> offscreenQuad;
	GLuint framebuffer, colorAttachment, renderbuffer;

	Example()
	{
		title = "offscreen rendering";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/15.offscreen_rendering/";
	}
	~Example()
	{

	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		// setup camera
		mMainCamera->setMoveSensitivity(20.0f);
		mMainCamera->setPosition(glm::vec3(0.0f, 15.0f, 40.0f));
		mMainCamera->setRotation(glm::vec3(-60.0f, 0.0f, 0.0f));

		std::vector<float> vertexAttribs = {
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
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexAttribs.size() / 5); i++)
		{
			Vertex vertex;
			vertex.vPosition = glm::vec3(vertexAttribs[i * 5], vertexAttribs[i * 5 + 1], vertexAttribs[i * 5 + 2]);
			vertex.vTexcoord = glm::vec2(vertexAttribs[i * 5 + 3], vertexAttribs[i * 5 + 4]);
			vertices.push_back(vertex);
		}

		model = Model::createFromFile("model",
			modelsDirectory + "/construction-site-rawscan/site.obj",
			{
				shadersDirectory + "construction.vert",
				shadersDirectory + "construction.frag"
			}
		);
		model->setRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
		/*
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
		*/
	}

	virtual void render(float deltaTime) override
	{
		SDL_GetWindowSize(window, &destWidth, &destHeight);
		glViewport(0, 0, destWidth, destHeight);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		/*
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
		*/
		model->render();
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