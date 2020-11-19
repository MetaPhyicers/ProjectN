#include "Engine.h"
#include "Resources.h"
#include "Windows.h"
#include "Utility.h"


Engine::Engine()
	:resources(new Resources()), window(nullptr)
{
}

Engine::~Engine()
{
	SafeDelete(resources);
	SafeDelete(window);
}

Window* Engine::CreateWindowX(double width, double height, const wchar_t* title, bool fullscreen)
{
	window = new Window(width, height, title, fullscreen);
	try
	{
		window->Init();
		return window;
	}
	catch (const std::exception&)
	{
		return nullptr;
	}

	return nullptr;
}

int Engine::Running()
{
	if (window) {
		return window->Running();
	}

	return 0;
}
