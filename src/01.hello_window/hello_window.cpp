#include <examplebase.h>

using namespace es;

class Example : public ExampleBase
{
public:
	Example()  
	{
		title = "hello window";
		settings.vsync = true;
	}

	~Example()
	{

	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();
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
	return EXIT_SUCCESS;
}
