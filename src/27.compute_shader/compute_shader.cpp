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
		title = "compute shader";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/27.compute_shader/";
		texturesDirectory = getResourcesPath(ResourceType::Texture) + "/27.compute_shader/";
	}

	~Example()
	{
		
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		computeProgram = Program::createFromFiles("compute_program", 
			{
				shadersDirectory + "emboss.comp"
	        }
		);

		inputQuad = Model::createFromFile("input_quad", modelsDirectory + "/quadrangle/quadrangle.obj",
			{
				shadersDirectory + "texture.vert",
				shadersDirectory + "texture.frag"
			}
		);
		
		std::shared_ptr<Texture2D> inputImage = Texture2D::createFromFile(texturesDirectory + "opengles.jpg", 1, false);
		inputQuad->setTexture("inputImage", inputImage);
		inputQuad->setPosition(glm::vec3(0.0f, 1.05f, 0.0f));
		inputQuad->setScale(glm::vec3(2.0f, 1.0f, 0.9f));
		
		outputQuad = Model::clone("output_quad", inputQuad.get());
		outputQuad->setPosition(glm::vec3(0.0f, -1.05f, 0.0f));
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