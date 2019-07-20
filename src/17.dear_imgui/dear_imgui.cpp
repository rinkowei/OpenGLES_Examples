
#include <common.h>

class Example : public ExampleBase
{
public:
	Example()
	{

	}
	~Example()
	{

	}
public:
	virtual void render()
	{

	}
};

Example *example;																																							\
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)						
{			
	example = new Example();															
    example->setupGLFW();
	example->loadGLESFunctions();
	example->setupWindow();													            
	example->prepare();																	
	example->renderLoop();																
	delete(example);																	
	return 0;																			
}	