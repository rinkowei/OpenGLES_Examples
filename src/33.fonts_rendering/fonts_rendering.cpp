#include <examplebase.h>
#include <textRenderer.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::unique_ptr<TextRenderer> textRenderer;

	Example()
	{
		title = "fonts rendering";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/33.fonts_rendering/";
		texturesDirectory = getResourcesPath(ResourceType::Texture);
		fontsDirectory = getResourcesPath(ResourceType::Font) + "/arial.ttf";
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

		std::shared_ptr<Material> fontMat = Material::createFromFiles("font_mat",
			{
				{ shadersDirectory + "text.vert" },
				{ shadersDirectory + "text.frag" },
			},
			{

			}
		);

		textRenderer = TextRenderer::create(mWindowWidth, mWindowHeight, fontMat);
		textRenderer->load(fontsDirectory, 96);
	}

	virtual void render(float deltaTime) override
	{
		textRenderer->text("rinkowei", 500.0f, 200.0f, 1.0f, glm::vec3(0.8f, 0.0f, 0.0f));
		textRenderer->text("Hello World!", 500.0f, 300.0f, 1.0f, glm::vec3(0.0f, 0.8f, 0.0f));
		textRenderer->text("OpenGL ES", 500.0f, 400.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.8f));
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