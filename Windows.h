#pragma once
#include "Utility.h"
#include "Renderer.h"

class Window;
class Clock;

class Window
{
public:
	typedef void(*KeyBoardFunc)(int key, int action);
	typedef void(*CursorPosFunc)(double xpos, double ypos);
	typedef void(*MouseFunc)(int button, int action, double xpos, double ypos, double scrollXOffset, double scrollYOffset);


	Window();
	Window(double width, double height, const wchar_t* title, bool fullscreen);
	~Window();

	// set window attributes
	void SetSize(int width, int height);
	void SetViewPort(int x, int y, int width, int height);
	void SetTitle(const wchar_t* title);
	void SetFullScreen(bool fullScreen);

	// set callback
	void SetKeyBoradFunc(KeyBoardFunc keyBoardFunc);
	void SetMouseFunc(MouseFunc mouseFunc);

	static double GetWindowWidth();
	static double GetWindowHeight();
	static const wchar_t* GetTitle();
	static float GetFPS();

	// Create renderer
	Graphic::Renderer* GetRenderer();

private:

	GLFWwindow* glfwWindow;
	HINSTANCE hInstance;
	std::wstring title;

	double width;
	double height;

	// system clock
	Clock* clock;
	// renderer
	Graphic::Renderer* renderer;

	bool isInitailized;
	bool isFullScreened;

	// Mouse attributes
	struct Mouse {
		// cursor pos
		double cursorXPos;
		double cursorYPos;
		// button
		int button;
		int action;
		// scroll
		double scollXOffset;
		double scollYOffset;
	};
	Mouse mouse;

	// input callback functions
	struct InputFuncs
	{
		KeyBoardFunc keyboardFunc;
		MouseFunc mouseFunc;
	};
	InputFuncs inputFuncs;

	bool Init();
	int Running();

	static void ErrorCallBack(int errorCode, const char* errorMessage);
	static void FrameBufferSizeCallBack(GLFWwindow* window, int w, int h);
	static void WidowSizeCallBack(GLFWwindow* window, int w, int h);

	/**
	*	keyborad callback
	*/
	static void KeyCallBack(GLFWwindow* window, int key, int scanCode, int action, int mods);
	static void CharCallBack(GLFWwindow* window, int codePoint);

	/**
	*	mouse callback
	*/
	static void CursorPosCallBack(GLFWwindow* window, double xpos, double ypos);
	static void MouseButtonCallBack(GLFWwindow* window, int button, int action, int mods);
	static void ScrollCallBack(GLFWwindow* window, double xoffset, double yoffset);	


	friend class Graphic::Renderer;
	friend class Engine;

};

class Clock
{
public:
	Clock();
	~Clock();

	float GetElapsedTimeInSecond();
	float GetCPUCurrentTime();
	float GetFrameElapsedTime();
	float GetFPS();

	const wchar_t* GetDateInYYMMDD();
	const wchar_t* GetClockTimeInHHMMSS();

	void Update();
	void AccumulateFrames();

private:
	LARGE_INTEGER cpuNow;
	LARGE_INTEGER cpuPre;
	LARGE_INTEGER cpuFrequency;

	int frames;
	float elapsedTime;
	float frameElapesdTime;
	float frameTimeAccumulation;
	float fps;

};