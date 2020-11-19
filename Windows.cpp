#include "Windows.h"
#include "Renderer.h"
#include "Debug.h"

// main thread safety
Window* g_pWindow = nullptr;

//
//	class Window: main framwork, provide modern OpenGL context, manage periphral inputs.
//

Window::Window()
	:glfwWindow(nullptr), hInstance(nullptr), title(L"Window"), width(800.0), height(800.0), mouse(), clock(new Clock()),
	renderer(nullptr), isInitailized(false), isFullScreened(false)
{
	g_pWindow = this;
	hInstance = GetModuleHandle(nullptr);
}

Window::Window(double width, double height, const wchar_t* title, bool fullscreen)
	:glfwWindow(nullptr), hInstance(nullptr), title(title), width(width), height(height), mouse(), clock(new Clock()), 
	renderer(nullptr), isInitailized(false), isFullScreened(fullscreen)
{
	g_pWindow = this;
	hInstance = GetModuleHandle(nullptr);
}

Window::~Window()
{
	delete renderer;
	delete clock;

	glfwDestroyWindow(glfwWindow);
	glfwTerminate();
}

void Window::SetSize(int width, int height)
{
	glfwSetWindowSize(glfwWindow, width, height);
	return;
}

void Window::SetViewPort(int x, int y, int width, int height)
{
	if (isInitailized) {
		glViewport(x, y, width, height);
	}
}

void Window::SetTitle(const wchar_t* title)
{
	this->title = title;
}

void Window::SetFullScreen(bool fullScreen)
{
	if (glfwWindow == nullptr) {
		return;
	}
	if (isFullScreened != fullScreen) {
		isFullScreened = fullScreen;
	}

	//GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	//const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
	//if (isFullScreened) {
	//	glfwSetWindowMonitor(glfwWindow, monitor, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);
	//}
	//else {
	//	glfwSetWindowMonitor(glfwWindow, monitor, 0, 0, width, height, 0);
	//}

}

void Window::SetKeyBoradFunc(KeyBoardFunc keyBoardFunc)
{
	inputFuncs.keyboardFunc = keyBoardFunc;
}

void Window::SetMouseFunc(MouseFunc mouseFunc)
{
	inputFuncs.mouseFunc = mouseFunc;
}

bool Window::Init()
{
	if (isInitailized) {
		return true;
	}

	// set error callback
	glfwSetErrorCallback(ErrorCallBack);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLFW_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLFW_VERSION_MINOR);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);
	glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
	

	// toggle debug context in debug mode
#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif // _DEBUG

	glfwWindow = glfwCreateWindow(width, height, Unicode::UnicodeToMultibytes(title.c_str()).c_str(), nullptr, nullptr);
	// make current context
	glfwMakeContextCurrent(glfwWindow);
	if (glfwWindow == nullptr) {
		glfwTerminate();
		ASSERT(false);
	}
	glfwSetCursorPos(glfwWindow, width / 2, height / 2);

	glfwSetFramebufferSizeCallback(glfwWindow, FrameBufferSizeCallBack);
	glfwSetWindowSizeCallback(glfwWindow, WidowSizeCallBack);

	glfwSetKeyCallback(glfwWindow, KeyCallBack);
	glfwSetCursorPosCallback(glfwWindow, CursorPosCallBack);
	glfwSetMouseButtonCallback(glfwWindow, MouseButtonCallBack);

	/**
	*	Initialize glew
	*/

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		ASSERT(false);
	}
	//	To check if we successfully initialized a debug context
#ifdef _DEBUG
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		// initialize debug output
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(reinterpret_cast<GLDEBUGPROC>(Debug::GLDebugOutput), nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
	else {
		Debug::ShowMessage("Debug context initailize failed!");
	}
#endif // _DEBUG
	

	// application initialized
	isInitailized = true;

	return true;
}

double Window::GetWindowWidth()
{
	if (!g_pWindow)
		return 0.0;

	return g_pWindow->width;
}

double Window::GetWindowHeight()
{
	if (!g_pWindow)
		return 0.0;

	return g_pWindow->height;
}

const wchar_t* Window::GetTitle()
{
	if (!g_pWindow)
		return L"";

	return g_pWindow->title.c_str();
}

float Window::GetFPS()
{
	if (!g_pWindow)
		return 0.f;

	return g_pWindow->clock->GetFPS();
}

int Window::Running()
{
	if (!isInitailized) {
		return 0;
	}

	while (!glfwWindowShouldClose(glfwWindow))
	{
		clock->Update();
		clock->AccumulateFrames();

		glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1.f, 1.f, 1.f, 1.f);

		glEnable(GL_DEPTH_TEST);

		if (renderer) {
			renderer->Render(clock->GetFrameElapsedTime());
		}

		glfwPollEvents();
		glfwSwapBuffers(glfwWindow);


		///< input mouse events
		if (inputFuncs.mouseFunc) {
			inputFuncs.mouseFunc(mouse.button, mouse.action, mouse.cursorXPos, mouse.cursorYPos, mouse.scollXOffset, mouse.scollYOffset);
		}
	}

	return 0;
}

Graphic::Renderer* Window::GetRenderer()
{
	if (!renderer) {
		renderer = new Graphic::Renderer();
		return renderer;
	}

	return renderer;
}

void Window::ErrorCallBack(int errorCode, const char* errorMessage)
{
	if (errorCode != GLFW_NO_ERROR) {
		// report error(s) and break
		Debug::ShowMessage(errorMessage);
	}
}

void Window::FrameBufferSizeCallBack(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Window::WidowSizeCallBack(GLFWwindow* window, int w, int h)
{
	if (g_pWindow) {
		g_pWindow->width = static_cast<float>(w);
		g_pWindow->height = static_cast<float>(h);
	}
}

void Window::KeyCallBack(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	///< input key events
	if (g_pWindow->inputFuncs.keyboardFunc) {
		g_pWindow->inputFuncs.keyboardFunc(key, action);
	}
}

void Window::CharCallBack(GLFWwindow* window, int codePoint)
{
}

void Window::CursorPosCallBack(GLFWwindow* window, double xpos, double ypos)
{
	g_pWindow->mouse.cursorXPos = xpos;
	g_pWindow->mouse.cursorYPos = ypos;
}

void Window::MouseButtonCallBack(GLFWwindow* window, int button, int action, int mods)
{
	g_pWindow->mouse.button = button;
	g_pWindow->mouse.action = action;
}

void Window::ScrollCallBack(GLFWwindow* window, double xoffset, double yoffset)
{
	g_pWindow->mouse.scollXOffset = xoffset;
	g_pWindow->mouse.scollYOffset = yoffset;
}

//
//	class Clock: provide timing services
//

Clock::Clock()
	:cpuNow(), cpuPre(), cpuFrequency(), frames(0), elapsedTime(0.f), frameElapesdTime(0.f), frameTimeAccumulation(0.f),
	fps(0.f)
{
	Update();
}

Clock::~Clock()
{
}

float Clock::GetElapsedTimeInSecond()
{
	return elapsedTime;
}

float Clock::GetCPUCurrentTime()
{
	return static_cast<float>(cpuNow.QuadPart);
}

float Clock::GetFrameElapsedTime()
{
	return frameElapesdTime;
}

const wchar_t* Clock::GetDateInYYMMDD()
{
	return nullptr;
}

const wchar_t* Clock::GetClockTimeInHHMMSS()
{
	return nullptr;
}

float Clock::GetFPS()
{
	return fps;
}

void Clock::Update()
{
	QueryPerformanceFrequency(&cpuFrequency);
	QueryPerformanceCounter(&cpuNow);

	frameElapesdTime = static_cast<float>(cpuNow.QuadPart - cpuPre.QuadPart) / static_cast<float>(cpuFrequency.QuadPart);
	elapsedTime += frameElapesdTime;

	frameTimeAccumulation += frameElapesdTime;

	// calculate fps
	if (frameTimeAccumulation >= 1.f) {
		fps = static_cast<float>(frames) / frameTimeAccumulation;
		frameTimeAccumulation = 0.f;
		frames = 0;
	}

	cpuPre = cpuNow;
}

void Clock::AccumulateFrames()
{
	frames++;
}
