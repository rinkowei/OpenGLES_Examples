#pragma once

#ifdef _WIN32
#pragma comment(linker, "/subsystem:windows")
#include <Windows.h>
#include <fcntl.h>
#include <io.h>
#include <ShellScalingApi.h>
#endif

#include <iostream>
#include <chrono>
#include <sys/stat.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <string>
#include <array>
#include <numeric>

#include "UIOverlay.h"
#include "camera.h"
#include "benchmark.h"

using namespace std;

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
	void handleMouseMove(GLint xoffset, GLint yoffset);
protected:
	// frame counter to display fps
	GLuint frameCounter = 0;
	GLuint lastFPS = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp;
public:
	bool prepared = false;
	GLuint width = 1280;
	GLuint height = 720;

	UIOverlay uiOverlay;

	float frameTimer = 1.0f;
	const std::string getResourcesPath();

	Benchmark benchmark;

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

	bool paused = false;

	float rotationSpeed = 1.0f;
	float zoomSpeed = 1.0f;

	Camera camera;

	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 cameraPos = glm::vec3(0.0f);
	glm::vec2 mousePos;

	std::string title = "OpenGL_ES Example";
	std::string name = "ExampleBase";
	GLuint apiVersion = GL_ES_VERSION_3_2;

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
	HWND window;
	HINSTANCE windowInstance;
#endif

	// constrctor
	ExampleBase();

	// deconstructor
	virtual ~ExampleBase();

	bool initGLFW();

#if defined(_WIN32)
	void setupConsole(std::string title);
	void setupDPIAwareness();
	HWND setupWindow(HINSTANCE hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

	virtual void render() = 0;

	virtual void viewChanged();

	virtual void keyPressed(GLuint);

	virtual void mouseMoved(double x, double y, bool& handled);

	virtual void windowResized();

	virtual void prepare();

	void renderLoop();

	void renderFrame();

	void updateUIOverlay();

	void drawUI();

	virtual void onUpdateUIOverlay(UIOverlay* overlay);
};

// OS specific macros for the example main entry point
#if defined(_WIN32)
#define OPENGL_ES_EXAMPLE_MAIN()                                                                    \
Example* example;                                                                                   \
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)						\
{																									\
	if (example != NULL)																		    \
	{																								\
		example->handleMessages(hWnd, uMsg, wParam, lParam);									    \
	}																								\
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));												\
}                                                                                                   \
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)									\
{																									\
for (int32_t i = 0; i < __argc; i++) { Example::args.push_back(__argv[i]); };  			            \
	example = new Example();															            \
	example->initGLFW();																	        \
	example->setupWindow(hInstance, WndProc);													    \
	example->prepare();																	 	        \
	example->renderLoop();																	        \
	delete(example);																			    \
	return 0;																						\
}
#endif