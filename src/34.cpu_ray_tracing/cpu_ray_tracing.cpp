#include <examplebase.h>
#include <ray.h>
#include <image.h>
#include <math.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Example()
	{
		title = "cpu ray tracing";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/34.cpu_ray_tracing/";
		texturesDirectory = getResourcesPath(ResourceType::Texture);
	}
	~Example()
	{

	}
public:
	glm::vec3 color(const Ray& r)
	{
		glm::vec3 direction = glm::normalize(r.getDirection());
		float t = 0.5f * (direction.y + 1.0f);
		return (1.0f - t) * glm::vec3(1.0f, 1.0f, 1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f);
	}
	virtual void prepare() override
	{
		ExampleBase::prepare();

		Image img(400, 400, 4);
		for (uint32_t i = 0; i < 400; i++) {
			for (uint32_t j = 0; j < 400; j++) {
				if ((i + j) % 2 == 0)
					img.setPixel(i, j, Image::Pixel<float>(0.0f, 0.0f, 0.0f, 1.0f));
				else {
					img.setPixel(i, j, Image::Pixel<float>(10.0 + i / 400.0, 10.0 + i / 400.0, 0, 0.1));
				}
			}
		}
		img.saveAsPNG("test.png");
	}

	virtual void render(float deltaTime) override
	{
		
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