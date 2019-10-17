#include "examplebase.h"
#include "world.h"

namespace es
{
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
		resDir = ES_EXAMPLE_RESOURCES_DIR;
#else
		resDir = "./../resources/";
#endif
		return resDir + resName;
	}

	ExampleBase::ExampleBase()
	{

	}

	ExampleBase::~ExampleBase()
	{
		mMainCamera = nullptr;
		SDL_Quit();
	}

	bool ExampleBase::setupSDL()
	{
		SDL_SetMainReady();
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "failed to init SDL! error : %s\n", SDL_GetError());
			return false;
		}
		atexit(SDL_Quit);

		SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1");
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_EGL, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		window = SDL_CreateWindow(title.c_str(),
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			defaultWindowWidth, defaultWindowHeight,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (!window)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
				"failed to create the main window.", nullptr);
			return false;
		}

		context = SDL_GL_CreateContext(window);
		if (!context)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
				"failed to create an OpenGL context.", nullptr);
			return false;
		}

		if (settings.vsync == true)
		{
			if (SDL_GL_SetSwapInterval(-1) != 0)
			{
				SDL_GL_SetSwapInterval(1);
			}
		}
		else
		{
			SDL_GL_SetSwapInterval(0);
		}

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
		ImGui_ImplSDL2_InitForOpenGL(window, context);
		ImGui_ImplOpenGL3_Init(apiVersion.c_str());

		return true;
	}

	bool ExampleBase::loadGLESFunctions()
	{
		return true;
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
		World::getWorld()->createMainCamera(45.0f, 0.1f, 100.0f, (float)defaultWindowWidth / (float)defaultWindowHeight, glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		mMainCamera = World::getWorld()->getMainCamera();
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
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();

		// rendering objects
		render(frameTimer);

		// rendering imgui
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		frameCounter++;
		auto timeEnd = std::chrono::high_resolution_clock::now();
		auto timeDiff = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count();
		frameTimer = (float)timeDiff / 1000.0f;

		World::getWorld()->update(frameTimer);
		viewUpdated = true;

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
				SDL_SetWindowTitle(window, windowTitle.c_str());
			}
			frameCounter = 0;
			lastTimestamp = timeEnd;
		}
		// TODO: Cap UI overlay update rates
		updateOverlay();
	}

	void ExampleBase::renderLoop()
	{
		lastTimestamp = std::chrono::high_resolution_clock::now();

		while (!mIsApplicationQuit)
		{
			if (SDL_PollEvent(&event) != 0)
			{
				handleSDLEvent(event);
			}

			handleKeyboardInput();
			handleMouseMove();
			renderFrame();
			SDL_GL_SwapWindow(window);
		}

		// clean up
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

		SDL_DestroyWindow(window);
	}

	void ExampleBase::updateOverlay()
	{
		if (!settings.overlay)
			return;

		ImGuiIO& io = ImGui::GetIO();

		io.DisplaySize = ImVec2((float)defaultWindowWidth, (float)defaultWindowHeight);
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

	void ExampleBase::handleSDLEvent(const SDL_Event& event)
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		ImGuiIO& io = ImGui::GetIO();

		if (event.type == SDL_QUIT)
		{
			mIsApplicationQuit = true;
		}
	}

	void ExampleBase::handleKeyboardInput()
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.KeysDown[SDL_SCANCODE_ESCAPE])
		{
			mIsApplicationQuit = true;
		}

		if (ImGui::IsKeyDown(SDL_SCANCODE_W))
		{
			mMainCamera->setTranslationDelta(mMainCamera->getForward(), frameTimer * 10.0f);
		}
		if (ImGui::IsKeyDown(SDL_SCANCODE_S))
		{
			mMainCamera->setTranslationDelta(-mMainCamera->getForward(), frameTimer * 10.0f);
		}
		if (ImGui::IsKeyDown(SDL_SCANCODE_A))
		{
			mMainCamera->setTranslationDelta(-mMainCamera->getRight(), frameTimer * 10.0f);
		}
		if (ImGui::IsKeyDown(SDL_SCANCODE_D))
		{
			mMainCamera->setTranslationDelta(mMainCamera->getRight(), frameTimer * 10.0f);
		}

		if (ImGui::IsKeyReleased(SDL_SCANCODE_W) || ImGui::IsKeyReleased(SDL_SCANCODE_S) || ImGui::IsKeyReleased(SDL_SCANCODE_A) || ImGui::IsKeyReleased(SDL_SCANCODE_D))
		{
			mMainCamera->setTranslationDelta(glm::vec3(0.0f), 0.0f);
		}
	}

	void ExampleBase::handleMouseMove()
	{
		ImGuiIO& io = ImGui::GetIO();
		float deltaX = io.MouseDelta.x;
		float deltaY = io.MouseDelta.y;
	
		// mouse left button down
		if (ImGui::IsMouseDown(0))
		{
			mMainCamera->setRotationDelta(glm::vec3((float)(deltaY * 0.08f),
				(float)(deltaX * 0.08f),
				(float)(0.0f)));
		}
		else
		{
			mMainCamera->setRotationDelta(glm::vec3(0.0f, 0.0f, 0.0f));
		}

		// mouse right button down
		if (ImGui::IsMouseDown(1))
		{

		}
		// mouse middle button down
		if (ImGui::IsMouseDown(2))
		{

		}
	}

	void ExampleBase::windowResized()
	{

	}

	void ExampleBase::onUpdateUIOverlay(es::UIOverlay* overlay)
	{

	}
}