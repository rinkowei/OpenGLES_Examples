#ifndef EXAMPLEBASE_H_
#define EXAMPLEBASE_H_

#include "ogles.h"

//  global compilation flag configuring windows sdk headers
//  preventing inclusion of min and max macros clashing with <limits>
#define NOMINMAX 1

//  override byte to prevent clashes with <cstddef>
#define byte win_byte_override

#include <Windows.h> // gdi plus requires Windows.h
// ...includes for other windows header that may use byte...

//  Define min max macros required by GDI+ headers.
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#else
#error max macro is already defined
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#else
#error min macro is already defined
#endif

#include <gdiplus.h>

//  Undefine min max macros so they won't collide with <limits> header content.
#undef min
#undef max

//  Undefine byte macros so it won't collide with <cstddef> header content.
#undef byte

#ifdef _WIN32
#pragma comment(linker, "/subsystem:windows")
#include <fcntl.h>
#include <io.h>
#include <ShellScalingApi.h>
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
#include "UIOverlay.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>

namespace es
{
	class ExampleBase
	{
	private:
		// get window title with example name
		std::string getWindowTitle();
		bool viewUpdated = false;
		bool resizing = false;
		bool mIsApplicationQuit = false;

		void windowResize();
		void handleKeyboardInput();
		void handleMouseMove();
		void handleSDLEvent(const SDL_Event& event);

	protected:
		enum class ResourceType
		{
			Model,
			Shader,
			Texture
		};

		SDL_Window* window;
		SDL_GLContext context;
		SDL_Event event;

		// frame counter to display fps
		GLuint64 frameCounter = 0;
		GLuint lastFPS = 0;
		std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp;

		std::string modelsDirectory;
		std::string shadersDirectory;
		std::string texturesDirectory;

		Camera* mMainCamera;
	public:
		bool prepared = false;
		GLint windowWidth = 1280;
		GLint windowHeight = 720;

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
		//HWND windowHwnd;
		//HINSTANCE windowInstance;
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

		virtual void windowResized();

		virtual void prepare();

		virtual void render(float deltaTime) = 0;

		void renderLoop();

		void renderFrame();

		void updateOverlay();

		void drawUI();

		virtual void onUpdateUIOverlay(es::UIOverlay* overlay);
	};
}                                                                      

#endif