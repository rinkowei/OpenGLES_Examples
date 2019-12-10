#include <examplebase.h>
#include <ray.h>
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

		int nx = 200;
		int ny = 100;
		std::cout << "P3\n" << nx << " " << ny << "\n255\n";
		glm::vec3 lower_left_corner(-2.0, -1.0, -1.0);
		glm::vec3 horizontal(4.0, 0.0, 0.0);
		glm::vec3 vertical(0.0, 2.0, 0.0);
		glm::vec3 origin(0.0, 0.0, 0.0);
		for (int j = ny - 1; j >= 0; j--) {
			for (int i = 0; i < nx; i++) {
				float u = float(i) / float(nx);
				float v = float(j) / float(ny);
				Ray r(origin, lower_left_corner + u * horizontal + v * vertical);
				glm::vec3 col = color(r);
				int ir = int(255.99 * col[0]);
				int ig = int(255.99 * col[1]);
				int ib = int(255.99 * col[2]);

				std::cout << ir << " " << ig << " " << ib << "\n";
			}
		}
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