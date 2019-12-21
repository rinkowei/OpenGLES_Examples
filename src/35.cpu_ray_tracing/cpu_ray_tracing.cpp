#include <examplebase.h>
#include <model.h>
#include <texture.h>
#include <ray.h>
#include <math.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> quad;

	Example()
	{
		title = "cpu ray tracing";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/35.cpu_ray_tracing/";
		texturesDirectory = getResourcesPath(ResourceType::Texture);
	}
	~Example()
	{

	}
public:
	float hit_sphere(const glm::vec3& center, float radius, const Ray& r) {
		glm::vec3 oc = r.getOrigin() - center;
		float a = glm::dot(r.getDirection(), r.getDirection());
		float b = 2.0f * glm::dot(oc, r.getDirection());
		float c = glm::dot(oc, oc) - radius * radius;
		float discriminant = b * b - 4 * a * c;
		if (discriminant < 0)
		{
			return -1.0f;
		}
		else
		{
			return (-b - glm::sqrt(discriminant)) / (2.0f * a);
		}
	}

	glm::vec3 color(const Ray& r) 
	{
		float t = hit_sphere(glm::vec3(0.0f, 0.0f, -1.0f), 0.5f, r);
		if (t > 0.0f)
		{
			glm::vec3 N = glm::normalize(r.pointAtParameter(t) - glm::vec3(0.0f, 0.0f, -1.0f));
			return 0.5f * glm::vec3(N.x + 1.0f, N.y + 1.0f, N.z + 1.0f);
		}
		glm::vec3 normalizedDir = glm::normalize(r.getDirection());
		t = 0.5f * (-normalizedDir.y + 1.0f);
		return (1.0f - t) * glm::vec3(1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f);
	}
	virtual void prepare() override
	{
		ExampleBase::prepare();

		Image img(200, 100, 4);
		int nx = 200;
		int ny = 100;

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
				float ir = col.x;
				float ig = col.y;
				float ib = col.z;
				img.setPixel(i, j, Image::Pixel<float>(ir, ig, ib, 1.0f));
			}
		}
		img.saveAsPNG("test.png");
		
		std::shared_ptr<Texture2D> raytracingMap = Texture2D::createFromData(200, 100, 1, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, true);
		raytracingMap->setData(0, img.getData());
		raytracingMap->setMinFilter(GL_NEAREST);
		raytracingMap->setMagFilter(GL_NEAREST);
		raytracingMap->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		std::shared_ptr<Material> quadMat = Material::createFromData("quad_mat",
			{
				shadersDirectory + "text.vert",
				shadersDirectory + "text.frag"
			},
			{
				{ "raytracingMap", raytracingMap }
			}
		);
		quad = Model::createFromFile("quad", modelsDirectory + "/quadrangle/quadrangle.obj", {}, false);
		quad->setMaterial(quadMat);
		
	}

	virtual void render(float deltaTime) override
	{
		quad->render();
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