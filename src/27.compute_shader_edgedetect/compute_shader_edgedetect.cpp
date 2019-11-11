#include <examplebase.h>
#include <model.h>
#include <material.h>
#include <buffer.h>
#include <random>
#include <ctime>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Program> computeProgram;
	std::shared_ptr<Model> inputQuad;
	std::shared_ptr<Model> outputQuad;

	Example()
	{
		title = "compute shader edgedetect";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/27.compute_shader_edgedetect/";
		texturesDirectory = getResourcesPath(ResourceType::Texture) + "/27.compute_shader_edgedetect/";
	}

	~Example()
	{
		
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();
		
		glEnable(GL_DEPTH_TEST);
		
		computeProgram = Program::createFromFiles("compute_program", 
			{
				shadersDirectory + "edgedetect.comp"
	        }
		);

		inputQuad = Model::createFromFile("input_quad", modelsDirectory + "/quadrangle/quadrangle.obj",
			{
				shadersDirectory + "texture.vert",
				shadersDirectory + "texture.frag"
			}
		);
	
		outputQuad = Model::createFromFile("output_quad", modelsDirectory + "/quadrangle/quadrangle.obj",
			{
				shadersDirectory + "texture.vert",
				shadersDirectory + "texture.frag"
			}
		);


		std::shared_ptr<Texture2D> inputImage = Texture2D::createFromFile(texturesDirectory + "vulkan.png", 1, false, false);
		std::shared_ptr<Texture2D> intermediateImage = Texture2D::createFromData(inputImage->getWidth(), inputImage->getHeight(), 1, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, true);
		std::shared_ptr<Texture2D> outputImage = Texture2D::createFromData(inputImage->getWidth(), inputImage->getHeight(), 1, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, true);
		
		computeProgram->apply();
		inputImage->bindImage(0, 0, 0, GL_READ_ONLY, GL_RGBA8);
		intermediateImage->bindImage(1, 0, 0, GL_WRITE_ONLY, GL_RGBA8);
		
		glDispatchCompute(1, 1024, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		intermediateImage->bindImage(0, 0, 0, GL_READ_ONLY, GL_RGBA8);
		outputImage->bindImage(1, 0, 0, GL_WRITE_ONLY, GL_RGBA8);

		glDispatchCompute(1, 1024, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		
		computeProgram->unapply();

		inputQuad->setTexture("inputImage", inputImage);
		inputQuad->setPosition(glm::vec3(-1.8f, 0.0f, 0.0f));
		inputQuad->setScale(glm::vec3(1.8f, 1.8f, 1.0f));
		
		outputQuad->setTexture("inputImage", outputImage);
		outputQuad->setPosition(glm::vec3(1.8f, 0.0f, 0.0f));
		outputQuad->setScale(glm::vec3(1.8f, 1.8f, 1.0f));
	}

	virtual void render(float deltaTime) override
	{
		inputQuad->render();
		outputQuad->render();
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