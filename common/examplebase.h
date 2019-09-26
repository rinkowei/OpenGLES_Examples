#pragma once

#include "ogles.h"

#ifdef _WIN32
#pragma comment(linker, "/subsystem:windows")
#include <Windows.h>
#include <fcntl.h>
#include <io.h>
//#include <ShellScalingApi.h>
#endif

#include <iostream>
#include <chrono>
#include <sys/stat.h>
#include <memory>

#include <string>
#include <array>
#include <vector>
#include <numeric>

#include "world.h"
#include "object.h"
#include "camera.h"
#include "UIOverlay.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>

using namespace es;

namespace es
{
	class ExampleBase
	{
	private:
		// get window title with example name
		std::string getWindowTitle();
		bool viewUpdated = false;
		GLboolean resizing = false;

		void windowResize();
		void handleKeyboardInput();
		void handleMouseMove();

	protected:
		enum class ResourceType
		{
			Model,
			Shader,
			Texture
		};

		SDL_Window* window;
		SDL_GLContext context;

		// frame counter to display fps
		GLuint64 frameCounter = 0;
		GLuint lastFPS = 0;
		std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp;

		std::string modelsDirectory;
		std::string shadersDirectory;
		std::string texturesDirectory;

		Camera* camera;
	public:
		bool prepared = false;
		GLint defaultWindowWidth = 1280;
		GLint defaultWindowHeight = 720;

		GLint destWidth;
		GLint destHeight;

		es::UIOverlay uiOverlay;

		float frameTimer = 1.0f / 60.0f;
		const std::string getResourcesPath(ResourceType type);
	    
		struct Settings
		{
			bool validation = false;
			bool fullscreen = false;
			bool vsync = false;
			bool overlay = false;
		} settings;

		glm::vec4 defaultClearColor = glm::vec4(0.025f, 0.025f, 0.025f, 1.0f);

		static std::vector<const char*> args;

		float timer = 0.0f;
		float timerSpeed = 0.25f;
		float timePassed = 0.0;

		GLboolean paused = false;

		std::string title = "OpenGL_ES Example";
		std::string name = "ExampleBase";
		std::string apiVersion = "#version 310 es";

		struct MouseButtons
		{
			bool left = false;
			bool right = false;
			bool middle = false;
		} mouseButtons;

#if defined(_WIN32)
		HWND windowHwnd;
		HINSTANCE windowInstance;
#endif

		// constrctor
		ExampleBase();

		// deconstructor
		virtual ~ExampleBase();
		 
		bool setupSDL();

		bool setupImGui();

		bool loadGLESFunctions();

		void setupValidation();

#if defined(_WIN32)
		void setupConsole(std::string title);
		void setupDPIAwareness();
#endif

		virtual void viewChanged();

		virtual void keyPressed(GLuint);

		virtual void mouseMoved(double x, double y, bool& handled);

		virtual void windowResized();

		virtual void handleInput();

		virtual void prepare();

		virtual void render(float deltaTime) = 0;

		void renderLoop();

		void renderFrame();

		void updateOverlay();

		void drawUI();

		virtual void onUpdateUIOverlay(es::UIOverlay* overlay);
	};
}                                                                      
