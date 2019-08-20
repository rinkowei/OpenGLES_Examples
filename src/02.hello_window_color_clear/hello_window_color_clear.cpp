
#include <common.h>

class Example : public ExampleBase
{
public:
	Example()
	{
		title = "hello window color clear";
		settings.vsync = true;
		defaultClearColor = glm::vec4(0.45f, 0.50f, 0.60f, 1.0f);
	}
	~Example()
	{

	}
public:
	virtual void update(float deltaTime) override
	{

	}
};

Example* example;
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	example = new Example();
	example->setupValidation();
	if (!example->setupGLFW() ||
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

