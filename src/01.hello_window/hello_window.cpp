
#include <common.h>

class Example : public ExampleBase
{
public:
	Example()
	{
		title = "hello window";
	}
	~Example()
	{

	}
public:
	virtual void render()
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
