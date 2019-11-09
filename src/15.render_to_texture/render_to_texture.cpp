#include <examplebase.h>
#include <model.h>
#include <material.h>
#include <buffer.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> model;
	std::shared_ptr<Mesh> offscreenQuad;

	std::unique_ptr<Framebuffer> frameBuffer;
	std::shared_ptr<Texture2D> renderTexture;
	std::unique_ptr<Renderbuffer> renderBuffer;

	Example()
	{
		title = "render to texture";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/15.render_to_texture/";
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
		mMainCamera->setPosition(glm::vec3(0.0f, 10.0f, 35.0f));

		std::vector<float> vertexAttribs = {
			 // positions       // texture coordinates
			 0.4f, 1.0f, 0.0f,  1.0f, 1.0f,
			 0.4f, 0.2f, 0.0f,  1.0f, 0.0f,
			-0.4f, 0.2f, 0.0f,  0.0f, 0.0f,
			-0.4f, 1.0f, 0.0f,  0.0f, 1.0f
		};

		std::vector<uint32_t> indices = {
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
			modelsDirectory + "/devils-slide-bunker/HW1_Bunker.obj",
			{
				shadersDirectory + "construction.vert",
				shadersDirectory + "construction.frag"
			}
		);
		model->setRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
		
		renderTexture = Texture2D::createFromData(mWindowWidth, mWindowHeight, -1, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, false);
		renderTexture->setMinFilter(GL_LINEAR);
		renderTexture->setMagFilter(GL_LINEAR);

		// create offscreenQuad material
		std::shared_ptr<Material> screenMat = Material::createFromData("screen_mat",
			{
				shadersDirectory + "screen.vert",
				shadersDirectory + "screen.frag"
			},
			{
				{ "renderTexture", renderTexture }
			}
		);

		// create offscreenQuad mesh
		offscreenQuad = Mesh::createWithData("offscreen_quad", vertices, indices);
		offscreenQuad->setDrawType(Mesh::DrawType::ELEMENTS);
		offscreenQuad->setMaterial(screenMat);

		// configure framebuffer
		frameBuffer = Framebuffer::create();
		frameBuffer->attachRenderTarget(0, renderTexture.get(), 0, 0);

		renderBuffer = Renderbuffer::create(GL_DEPTH24_STENCIL8, mWindowWidth, mWindowHeight);
		frameBuffer->attachRenderBufferTarget(renderBuffer.get());
	}

	virtual void render(float deltaTime) override
	{
		frameBuffer->bind();
		// clear color and depth buffer
		glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT  | GL_STENCIL_BUFFER_BIT);
		model->render();
		
		frameBuffer->unbind();
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		model->render();

		// diable depth test for render quad in front of scene
		glDisable(GL_DEPTH_TEST);
		offscreenQuad->render();
		glEnable(GL_DEPTH_TEST);
	}

	virtual void windowResized() override
	{
		ExampleBase::windowResized();

		renderTexture->resize(0, mWindowWidth, mWindowHeight);
		renderBuffer->resize(mWindowWidth, mWindowHeight);
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