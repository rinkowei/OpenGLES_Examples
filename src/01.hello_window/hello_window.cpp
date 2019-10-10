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
		SDL_GetWindowSize(window, &destWidth, &destHeight);
		glViewport(0, 0, destWidth, destHeight);
	}
};

Example* example;
int APIENTRY SDL_main(int args, char **argc)
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
