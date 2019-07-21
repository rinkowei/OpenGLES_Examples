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

bool ExampleBase::setupImGui()
{
	// setup dear imgui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	// enable keyboard controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

	// setup dear imgui style
	ImGui::StyleColorsDark();

	// setup platform/renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(apiVersion.c_str());

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
	auto timeStart = std::chrono::high_resolution_clock::now();
	if (viewUpdated)
	{
		viewUpdated = false;
		viewChanged();
	}

	// start the dear imgui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	render();

	// Rendering
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(defaultClearColor.x, defaultClearColor.y, defaultClearColor.z, defaultClearColor.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
	frameCounter++;
	auto timeEnd = std::chrono::high_resolution_clock::now();
	auto timeDiff = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count();
	frameTimer = (float)timeDiff / 1000.0f;
	camera.update(frameTimer);
	if (camera.moving())
	{
		viewUpdated = true;
	}

	if (!paused)
	{
		timer += timerSpeed * frameTimer;
		if (timer > 1.0f)
		{
			timer -= 1.0f;
		}
	}
	float fpsTimer = std::chrono::duration<double, std::milli>(timeEnd - lastTimestamp).count();
	if (fpsTimer > 1000.0f)
	{
		lastFPS = static_cast<uint32_t>((float)frameCounter * (1000.0f / fpsTimer));
		if (!settings.overlay) {
			std::string windowTitle = getWindowTitle();
			glfwSetWindowTitle(window, windowTitle.c_str());
		}
		frameCounter = 0;
		lastTimestamp = timeEnd;
	}
	// TODO: Cap UI overlay update rates
	updateOverlay();
}

void ExampleBase::renderLoop()
{
	if (benchmark.active)
	{
		benchmark.run([=] { render(); });
		if (benchmark.filename != "")
		{
			benchmark.saveResults();
		}
		return;
	}

	destWidth = width;
	destHeight = height;
	lastTimestamp = std::chrono::high_resolution_clock::now();

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		renderFrame();
		
		glfwSwapBuffers(window);
	}

	// clean up
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void ExampleBase::updateOverlay()
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