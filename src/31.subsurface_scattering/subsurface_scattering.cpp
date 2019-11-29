#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> bunny;

	const uint32_t depthMapSize = 2048;
	std::unique_ptr<Framebuffer> depthFBO;
	std::shared_ptr<Texture2D> depthMap;
	std::unique_ptr<Renderbuffer> renderbuffer;

	std::shared_ptr<Material> depthPassMat;
	std::shared_ptr<Material> sssMat;

	glm::vec3 lightDir = glm::normalize(glm::vec3(0.0f, -1.0f, -1.0f));

	glm::mat4 biasMatrix = glm::mat4(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);

	Example()
	{
		title = "subsurface scattering";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/31.subsurface_scattering/";
		texturesDirectory = getResourcesPath(ResourceType::Texture);
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

		// enable cull face
		glEnable(GL_CULL_FACE);

		depthFBO = Framebuffer::create();

		depthMap = Texture2D::createFromData(depthMapSize, depthMapSize, 1, 1, GL_RGBA32F, GL_RGBA, GL_FLOAT, true);
		depthMap->setMinFilter(GL_NEAREST);
		depthMap->setMagFilter(GL_NEAREST);
		depthMap->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		depthFBO->addAttachmentTexture2D(GL_COLOR_ATTACHMENT0, depthMap->getTarget(), depthMap->getID(), 0);
		
		renderbuffer = Renderbuffer::create(GL_DEPTH24_STENCIL8, depthMapSize, depthMapSize);
		depthFBO->addAttachmentRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, renderbuffer->getTarget(), renderbuffer->getID());

		std::array<GLenum, 1> bufs = { GL_COLOR_ATTACHMENT0 };
		depthFBO->drawBuffers(bufs.size(), bufs.data());

		bunny = Model::createFromFile("bunny", modelsDirectory + "/bunny/bunny.obj", {}, false);

		depthPassMat = Material::createFromFiles("depth_pass_mat",
			{
				shadersDirectory + "depth_pass.vert",
				shadersDirectory + "depth_pass.frag"
			},
			{
			
			}
		);

		sssMat = Material::createFromData("sss_mat",
			{
				shadersDirectory + "bssrdf.vert",
				shadersDirectory + "bssrdf.frag"
			},
			{
				{ "depthMap", depthMap }
			}
		);
		sssMat->setUniform("biasMatrix", biasMatrix);
		sssMat->setUniform("lightDir", lightDir);
	}

	virtual void render(float deltaTime) override
	{
		depthFBO->bind();
		glViewport(0, 0, depthMapSize, depthMapSize);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_BACK);

		bunny->setMaterial(depthPassMat);
		glm::mat4 lightView = glm::lookAt(-lightDir, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightProj = glm::ortho(-2.5f, 2.5f, -2.5f, 2.5f, -10.0f, 20.0f);

		bunny->setUniform("lightView", lightView);
		bunny->setUniform("lightProj", lightProj);
		bunny->render();

		depthFBO->unbind();
		glViewport(0, 0, mWindowWidth, mWindowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		bunny->setMaterial(sssMat);
		bunny->setUniform("lightView", lightView);
		bunny->setUniform("lightProj", lightProj);
		bunny->setUniform("viewPos", mMainCamera->getPosition());
		bunny->render();
	}

	virtual void windowResized() override
	{
		ExampleBase::windowResized();
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