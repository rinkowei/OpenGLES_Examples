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
	GLuint tex;
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

		outputQuad = Model::createFromFile("output_quad", modelsDirectory + "/quadrangle/quadrangle.obj",
			{
				shadersDirectory + "texture.vert",
				shadersDirectory + "texture.frag"
			}
		);


		std::shared_ptr<Texture2D> inputImage = Texture2D::createFromFile(texturesDirectory + "opengles.jpg", 1, false);

		std::shared_ptr<Texture2D> outputImage = Texture2D::createFromData(inputImage->getWidth(), inputImage->getHeight(), 1, 1, 1, GL_RGBA32F, GL_RGBA, GL_FLOAT);
		//outputImage->bindImage(0, 0, 0, GL_WRITE_ONLY, GL_RGBA32F);
	
		tex = 0;

		const unsigned int w = inputImage->getWidth();
		const unsigned int h = inputImage->getHeight();

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);

		/* ComputeShader need to use immutable image space */
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);
		
		computeProgram->apply();
		inputImage->bindImage(0, 0, 0, GL_READ_ONLY, GL_RGBA8);
		glBindImageTexture(1, tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
		glUniform1i(glGetUniformLocation(computeProgram->getID(), "inputImage"), 0);
		glUniform1i(glGetUniformLocation(computeProgram->getID(), "outputImage"), 1);
		
		glDispatchCompute(w, h, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		
		computeProgram->unapply();

		inputQuad->setTexture("inputImage", inputImage);
		inputQuad->setPosition(glm::vec3(0.0f, 1.05f, 0.0f));
		inputQuad->setScale(glm::vec3(2.0f, 1.0f, 0.9f));
		
		//outputQuad->setTexture("inputImage", outputImage);
		outputQuad->setPosition(glm::vec3(0.0f, -1.05f, 0.0f));
		outputQuad->setScale(glm::vec3(2.0f, 1.0f, 0.9f));
	}

	virtual void render(float deltaTime) override
	{
		inputQuad->render();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
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