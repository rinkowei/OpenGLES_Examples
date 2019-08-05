// Implementation of Common library

#include "examplebase.h"

std::vector<const char*> ExampleBase::args;

std::string ExampleBase::getWindowTitle()
{
	return title;
}

const std::string ExampleBase::getResourcesPath(ResourceType type)
{
	std::string resName = "";
	switch (type)
	{
	case ResourceType::Model:
		resName = "models";
		break;
	case ResourceType::Shader:
		resName = "shaders";
		break;
	case ResourceType::Texture:
		resName = "textures";
		break;
	}

	std::string resDir = "";
#if defined(ES_EXAMPLE_RESOURCES_DIR)
	resDir =  ES_EXAMPLE_RESOURCES_DIR;
#else
	resDir = "./../resources/";
#endif
	return resDir + resName;
}

ExampleBase::ExampleBase()
{
	camera.setPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
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
	glfwWindowHint(GLFW_OPENGL_ES_API, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	
	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!window)
		return false;
	glfwMakeContextCurrent(window);

	if (this->settings.vsync)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);

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
	io.WantCaptureKeyboard = false;

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

void ExampleBase::setupValidation()
{
#if defined(_WIN32)
	if (this->settings.validation)
	{
		setupConsole("OpenGL ES validation output");
	}
#endif
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

	// Rendering
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(defaultClearColor.x, defaultClearColor.y, defaultClearColor.z, defaultClearColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (size_t i = 0; i < objectPool.size(); i++)
	{
		objectPool[i]->Draw();
	}

	render();

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
		timePassed += frameTimer;
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

void ExampleBase::addObject(Object* obj)
{
	objectPool.push_back(obj);
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
		handleInput();
		renderFrame();
		glfwSwapBuffers(window);
		glfwPollEvents();
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
	if (!settings.overlay)
		return;

	ImGuiIO& io = ImGui::GetIO();

	io.DisplaySize = ImVec2((float)width, (float)height);
	io.DeltaTime = frameTimer;
}

void ExampleBase::drawUI()
{

}

#if defined(_WIN32)
void ExampleBase::setupConsole(std::string title)
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w+", stdout);
	freopen_s(&stream, "CONOUT$", "w+", stderr);
	SetConsoleTitle(TEXT(title.c_str()));
}

void ExampleBase::setupDPIAwareness()
{
	/*
	typedef HRESULT* (__stdcall * SetProcessDpiAwarenessFunc)(PROCESS_DPI_AWARENESS);

	HMODULE shCore = LoadLibraryA("Shcore.dll");
	if (shCore)
	{
		SetProcessDpiAwarenessFunc setProcessDpiAwareness =
			(SetProcessDpiAwarenessFunc)GetProcAddress(shCore, "SetProcessDpiAwareness");

		if (setProcessDpiAwareness != nullptr)
		{
			setProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
		}

		FreeLibrary(shCore);
	}
	*/
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

void ExampleBase::handleInput()
{
	handleKeyboardInput();
	handleMouseMove();
}

void ExampleBase::handleKeyboardInput()
{
	// handle keyboard input
	if (ImGui::IsKeyPressed(GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);
	
	if (ImGui::IsKeyPressed(GLFW_KEY_P))
		paused = !paused;

	if (ImGui::IsKeyPressed(GLFW_KEY_W))
		camera.keys.up = true;
	if (ImGui::IsKeyPressed(GLFW_KEY_S))
		camera.keys.down = true;
	if (ImGui::IsKeyPressed(GLFW_KEY_A))
		camera.keys.left = true;
	if (ImGui::IsKeyPressed(GLFW_KEY_D))
		camera.keys.right = true;
	
	if (ImGui::IsKeyReleased(GLFW_KEY_W))
		camera.keys.up = false;
	if (ImGui::IsKeyReleased(GLFW_KEY_S))
		camera.keys.down = false;
	if (ImGui::IsKeyReleased(GLFW_KEY_A))
		camera.keys.left = false;
	if (ImGui::IsKeyReleased(GLFW_KEY_D))
		camera.keys.right = false;
}

void ExampleBase::handleMouseMove()
{
	ImGuiIO& io = ImGui::GetIO();
	float deltaX = io.MouseDelta.x;
	float deltaY = io.MouseDelta.y;
	// mouse left button down
	if (ImGui::IsMouseDown(0))
	{
		rotation.x += deltaY * 1.25f * rotationSpeed;
		rotation.y -= deltaX * 1.25f * rotationSpeed;
		camera.rotate(glm::vec3(deltaY * camera.rotationSpeed, deltaX * camera.rotationSpeed, 0.0f));
		viewUpdated = true;
	}
	// mouse right button down
	if (ImGui::IsMouseDown(1))
	{
		zoom += deltaY * 0.005f * zoomSpeed;
		camera.translate(glm::vec3(-0.0f, 0.0f, deltaY * 0.005f * zoomSpeed));
		viewUpdated = true;
	}
	// mouse middle button down
	if (ImGui::IsMouseDown(2))
	{
		cameraPos.x -= deltaX * 0.01f;
		cameraPos.y -= deltaY * 0.01f;
		camera.translate(glm::vec3(-deltaX * 0.01f, deltaY * 0.01f, 0.0f));
		viewUpdated = true;
	}
}

void ExampleBase::windowResized()
{

}

void ExampleBase::onUpdateUIOverlay(es::UIOverlay* overlay)
{

}