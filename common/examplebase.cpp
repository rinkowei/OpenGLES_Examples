// Implementation of Common library

#include "examplebase.h"

std::vector<const char*> ExampleBase::args;

std::string ExampleBase::getWindowTitle()
{
	return title;
}

const std::string ExampleBase::getResourcesPath()
{
#if defined(ES_EXAMPLE_RESOURCES_DIR)
	return ES_EXAMPLE_RESOURCES_DIR;
#else
	return "./../resources/";
#endif
}

ExampleBase::ExampleBase()
{

}

ExampleBase::~ExampleBase()
{

}

bool ExampleBase::setupGLFW()
{
	if (!glfwInit())
		return false;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!window)
		return false;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	return true;
}

bool ExampleBase::loadGLESFunctions()
{
	if (gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Success to initialize GLAD\n";
		return true;
	}
	else
	{
		std::cout << "Failed to initialize GLAD\n";
	}
	return false;
}

void ExampleBase::prepare()
{

}

void ExampleBase::renderFrame()
{

}

void ExampleBase::renderLoop()
{
	while (true)
	{
		int i = 0;
		i++;
	}
}

void ExampleBase::updateUIOverlay()
{

}

void ExampleBase::drawUI()
{

}

#if defined(_WIN32)
void ExampleBase::setupConsole(std::string title)
{

}

void ExampleBase::setupDPIAwareness()
{

}

void ExampleBase::setupWindow()
{

}

void ExampleBase::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

}
#endif

void ExampleBase::viewChanged()
{

}

void ExampleBase::keyPressed(GLuint)
{

}
void ExampleBase::mouseMoved(double x, double y, bool& handled)
{

}

void ExampleBase::windowResize()
{

}

void ExampleBase::handleMouseMove(GLint xoffset, GLint yoffset)
{

}

void ExampleBase::windowResized()
{

}

void ExampleBase::onUpdateUIOverlay(es::UIOverlay* overlay)
{

}