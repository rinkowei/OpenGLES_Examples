#pragma once

#define GLFW_INCLUDE_ES32
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

#include "object.h"
#include "camera.h"
#include "benchmark.h"
#include "UIOverlay.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengles32.h>

using namespace es;

namespace es
{
	class ExampleBase
	{
	private:
		// get window title with example name
		std::string getWindowTitle();
		bool viewUpdated = false;

		GLuint destWidth;
		GLuint destHeight;
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
		GLFWwindow* window;
		// frame counter to display fps
		GLuint64 frameCounter = 0;
		GLuint lastFPS = 0;
		std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp;

		std::string modelsDirectory;
		std::string shadersDirectory;
		std::string texturesDirectory;

		std::vector<Object*> objectPool;
	public:
		bool prepared = false;
		GLuint width = 1280;
		GLuint height = 720;

		es::UIOverlay uiOverlay;

		float frameTimer = 1.0f;
		const std::string getResourcesPath(ResourceType type);

		es::Benchmark benchmark;

		struct Settings
		{
			bool validation = false;
			bool fullscreen = false;
			bool vsync = false;
			bool overlay = false;
		} settings;

		glm::vec4 defaultClearColor = glm::vec4(0.025f, 0.025f, 0.025f, 1.0f);

		float zoom = 0.0f;

		static std::vector<const char*> args;

		float timer = 0.0f;
		float timerSpeed = 0.25f;
		float timePassed = 0.0;

		GLboolean paused = false;

		float rotationSpeed = 1.0f;
		float zoomSpeed = 1.0f;

		es::Camera camera;

		glm::vec3 rotation = glm::vec3(0.0f);
		glm::vec3 cameraPos = glm::vec3(0.0f);
		glm::vec2 mousePos;

		std::string title = "OpenGL_ES Example";
		std::string name = "ExampleBase";
		std::string apiVersion = "#version 320 es";

		struct GamePadState
		{
			glm::vec2 axisLeft = glm::vec2(0.0f);
			glm::vec2 axisRight = glm::vec2(0.0f);
		} gamePadState;

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

		bool setupGLFW();

		bool setupImGui();

		bool loadGLESFunctions();

		void setupValidation();

#if defined(_WIN32)
		void setupConsole(std::string title);
		void setupDPIAwareness();
#endif

		void addObject(Object* obj);

		virtual void render() = 0;

		virtual void viewChanged();

		virtual void keyPressed(GLuint);

		virtual void mouseMoved(double x, double y, bool& handled);

		virtual void windowResized();

		virtual void handleInput();

		virtual void prepare();

		void renderLoop();

		void renderFrame();

		void updateOverlay();

		void drawUI();

		virtual void onUpdateUIOverlay(es::UIOverlay* overlay);
	};
}                                                                      
