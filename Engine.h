#pragma once

class Window;
class Resources;

class Engine
{
public:
	Engine();
	~Engine();

	Window* CreateWindowX(double width, double height, const wchar_t* title = L"Window", bool fullscreen = false);

	int Running();

private:
	Resources* resources;
	Window* window;
};

